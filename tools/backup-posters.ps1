<#
.SYNOPSIS
  Backs up the movie poster content in public\ to another drive.

.DESCRIPTION
  Two modes (they can be combined):

  Mirror (default)  Keeps an exact copy of public\ at <Destination>\Mirror using robocopy.
                    Only new or changed files are copied, so after the first run it takes
                    seconds. Files you delete from public\ are also deleted from the
                    mirror, so it always matches the source exactly.

  -Zip              Writes a dated archive to <Destination>\Archives, e.g.
                    posters-2026-11-12_1430.zip. Images are stored without compression
                    because JPEGs barely shrink; this makes it much faster. Only the
                    newest -Keep archives are kept.

.EXAMPLE
  .\backup-posters.ps1 -Destination E:\Backups\Movie-Posters
  .\backup-posters.ps1 -Destination E:\Backups\Movie-Posters -Zip
  .\backup-posters.ps1 -Destination E:\Backups\Movie-Posters -Zip -NoMirror -Keep 3
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$Destination,

    [string]$Source,

    [switch]$Zip,
    [switch]$NoMirror,
    [int]$Keep = 5
)

$ErrorActionPreference = 'Stop'
# $PSScriptRoot isn't available in param() defaults on Windows PowerShell 5.1
if (-not $Source) { $Source = Join-Path $PSScriptRoot '..\public' }
$Source = (Resolve-Path $Source).Path

if ($NoMirror -and -not $Zip) {
    throw 'Nothing to do: -NoMirror was given without -Zip.'
}

$srcRoot  = [IO.Path]::GetPathRoot($Source)
$destFull = [IO.Path]::GetFullPath($Destination)
if ($destFull.StartsWith($Source, [StringComparison]::OrdinalIgnoreCase)) {
    throw "Destination must not be inside the source folder ($Source)."
}
if ([IO.Path]::GetPathRoot($destFull) -eq $srcRoot) {
    Write-Warning "Destination is on the same drive as the source ($srcRoot). That won't protect you from a drive failure."
}

New-Item -ItemType Directory -Force -Path $destFull | Out-Null
$logDir = Join-Path $destFull 'Logs'
New-Item -ItemType Directory -Force -Path $logDir | Out-Null
$stamp = Get-Date -Format 'yyyy-MM-dd_HHmm'

Write-Host "Source:      $Source"
Write-Host "Destination: $destFull"
Write-Host ''

# ---- Mirror -------------------------------------------------------------
if (-not $NoMirror) {
    $mirror = Join-Path $destFull 'Mirror'
    $log    = Join-Path $logDir "mirror-$stamp.log"
    Write-Host "Mirroring to $mirror ..."

    # /MIR  mirror the tree (copies new/changed, removes deleted)
    # /FFT  2-second timestamp tolerance (safe for exFAT/NAS targets)
    # /R:2 /W:5  don't retry forever on locked files
    # /MT:16  copy with 16 threads
    robocopy $Source $mirror /MIR /FFT /R:2 /W:5 /MT:16 /NP /NDL /NFL /TEE /LOG:$log | Out-Host

    # robocopy exit codes 0-7 are success; 8+ means something failed
    if ($LASTEXITCODE -ge 8) {
        throw "robocopy reported errors (exit code $LASTEXITCODE). See $log"
    }
    Write-Host "Mirror complete. Log: $log" -ForegroundColor Green
    Write-Host ''
}

# ---- Zip archive --------------------------------------------------------
if ($Zip) {
    $archiveDir = Join-Path $destFull 'Archives'
    New-Item -ItemType Directory -Force -Path $archiveDir | Out-Null
    $zipPath = Join-Path $archiveDir "posters-$stamp.zip"
    $tmpPath = "$zipPath.partial"

    Write-Host "Creating $zipPath ..."
    # Windows' built-in tar.exe writes .zip files and handles archives over 4 GB,
    # unlike Compress-Archive. "store" skips compressing the JPEGs.
    tar.exe -a -c --options zip:compression=store -f $tmpPath -C $Source .
    if ($LASTEXITCODE -ne 0) {
        Remove-Item $tmpPath -ErrorAction SilentlyContinue
        throw "tar failed (exit code $LASTEXITCODE)."
    }
    Move-Item $tmpPath $zipPath

    $sizeGB = (Get-Item $zipPath).Length / 1GB
    Write-Host ("Archive complete: {0:N2} GB" -f $sizeGB) -ForegroundColor Green

    # Prune old archives
    $old = Get-ChildItem $archiveDir -Filter 'posters-*.zip' |
           Sort-Object Name -Descending | Select-Object -Skip $Keep
    foreach ($f in $old) {
        Write-Host "Removing old archive $($f.Name)"
        Remove-Item $f.FullName
    }
}

# ---- Summary ------------------------------------------------------------
$srcCount = (Get-ChildItem $Source -Recurse -File).Count
Write-Host ''
Write-Host "Done. $srcCount files in source." -ForegroundColor Cyan
