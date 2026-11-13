<template>
  <div id="app">
    <div :class='headerClass' @click="toggleSettings">
      <h1>{{ displayMode }}</h1>
    </div>
    <div :class='posterClass' @click="toggleSearch">
      <img class='poster-img' :src="posterImage">
    </div>
    <div :class='footerClass' @click="toggleSources">
      <img class='footer-img' :src='sourceImage'>
    </div>
    
    <!-- Pause Indicator -->
    <transition name="fade">
      <div v-if="isPaused" class="pause-indicator">
        <div class="pause-icon">
          <div class="pause-bar"></div>
          <div class="pause-bar"></div>
        </div>
        <div class="pause-text">PAUSED</div>
      </div>
    </transition>
    
    <div class='settings-div' v-if="showSettings">
      <Settings @close="toggleSettings"></Settings>
    </div>
    <div class='search-div' v-if="showSearch">
      <Search @close="toggleSearch"></Search>
    </div>
    <div class='sources-div' v-if="showSources">
      <Sources @close="toggleSources"></Sources>
    </div>
  </div>
</template>

<script>
import { computed, ref, onMounted, onBeforeUnmount, watch } from 'vue'
import { useMovieStore } from './stores/movieStore.js'
import Settings from './components/Settings.vue'
import Sources from './components/Source.vue'
import Search from './components/Search.vue'

export default {
  name: 'App',
  components: { Settings, Search, Sources },
  setup() {
    const movieStore = useMovieStore()
    
    // UI state
    const showSettings = ref(false)
    const showSearch = ref(false)
    const showSources = ref(false)
    const timerId = ref(-1)
    const isPaused = ref(false)

    // Computed properties that pull from store
    const displayMode = computed(() => movieStore.displayMode)
    const posterSize = computed(() => movieStore.posterSize)
    const posterImage = computed(() => movieStore.posterImage)
    const cyclePosters = computed(() => movieStore.cyclePosters)
    const sourceImage = computed(() => movieStore.sourceImage)
    const displayDuration = computed(() => movieStore.displayDurationMs)

    // CSS class bindings
    const headerClass = computed(() => ({
      header27x41: posterSize.value === '27x41',
      header27x40: posterSize.value === '27x40'
    }))

    const posterClass = computed(() => ({
      poster27x41: posterSize.value === '27x41',
      poster27x40: posterSize.value === '27x40'
    }))

    const footerClass = computed(() => ({
      footer27x41: posterSize.value === '27x41',
      footer27x40: posterSize.value === '27x40'
    }))

    // Methods
    const nextMovie = () => {
      movieStore.gotoNextMovie()
    }

    const nextPoster = () => {
      movieStore.gotoNextPoster()
    }

    const setupTimer = () => {
      // Clear existing timer
      if (timerId.value !== -1) {
        clearInterval(timerId.value)
        timerId.value = -1
      }

      // Don't setup timer if paused
      if (isPaused.value) {
        return
      }

      // Setup new timer based on mode
      if (displayMode.value === 'COMING SOON') {
        timerId.value = setInterval(nextMovie, displayDuration.value)
      } else if (displayMode.value === 'NOW SHOWING' && cyclePosters.value) {
        timerId.value = setInterval(nextPoster, displayDuration.value)
      }
    }

    const pauseSlideshow = () => {
      isPaused.value = true
      if (timerId.value !== -1) {
        clearInterval(timerId.value)
        timerId.value = -1
      }
    }

    const resumeSlideshow = () => {
      isPaused.value = false
      setupTimer()
    }

    const togglePause = () => {
      if (isPaused.value) {
        resumeSlideshow()
      } else {
        pauseSlideshow()
      }
    }

    const closeAllModals = () => {
      showSettings.value = false
      showSearch.value = false
      showSources.value = false
    }

    const handleKeyboard = (event) => {
      // Don't handle if typing in an input field
      if (event.target.tagName === 'INPUT' || event.target.tagName === 'TEXTAREA') {
        return
      }

      switch(event.key) {
        case 'Escape':
          // Close any open modal
          closeAllModals()
          break
          
        case ' ':
        case 'Spacebar':
          // Toggle pause/play
          event.preventDefault() // Prevent page scroll
          togglePause()
          break
          
        case 'ArrowRight':
          // Next movie or poster
          event.preventDefault()
          if (displayMode.value === 'COMING SOON') {
            movieStore.gotoNextMovie()
          } else {
            movieStore.gotoNextPoster()
          }
          break
          
        case 'ArrowLeft':
          // Previous poster (for NOW SHOWING mode)
          event.preventDefault()
          if (displayMode.value === 'COMING SOON') {
            movieStore.gotoPreviousMovie();
          } else {
            movieStore.gotoPreviousPoster()
          }
          break
      }
    }

    const toggleSettings = () => {
      showSettings.value = !showSettings.value
    }

    const toggleSearch = () => {
      showSearch.value = !showSearch.value
    }

    const toggleSources = () => {
      showSources.value = !showSources.value
    }

    // Watch for changes that require timer reset
    watch([displayMode, cyclePosters, displayDuration], () => {
      setupTimer()
    })

    // Initialize on mount
    onMounted(() => {
      movieStore.initMovies()
      setupTimer()
      
      // Add keyboard event listener
      window.addEventListener('keydown', handleKeyboard)
    })

    // Cleanup on unmount
    onBeforeUnmount(() => {
      if (timerId.value !== -1) {
        clearInterval(timerId.value)
      }
      
      // Remove keyboard event listener
      window.removeEventListener('keydown', handleKeyboard)
    })

    return {
      // State
      displayMode,
      posterSize,
      posterImage,
      sourceImage,
      showSettings,
      showSearch,
      showSources,
      isPaused,
      
      // Computed
      headerClass,
      posterClass,
      footerClass,
      
      // Methods
      toggleSettings,
      toggleSearch,
      toggleSources
    }
  }
}
</script>

<style scoped>
:root {
  /* Color Variables */
  --color-black: #000000;
  --color-white: #FFFFFF;
  --color-primary: #2196F3;
  --color-primary-light: #87CEEB;
  --color-gray-dark: #696969;
  
  /* Spacing Variables */
  --border-width: 30px;
  --border-width-double: 60px;
  
  /* Poster Size Ratios for 27x41 */
  --ratio-27x41-header: 0.130;
  --ratio-27x41-footer: 1.65;
  --ratio-27x41-footer-height: 0.13;
  
  /* Poster Size Ratios for 27x40 */
  --ratio-27x40-header: 0.148;
  --ratio-27x40-footer: 1.63;
  --ratio-27x40-footer-height: 0.148;
}

#app {
  background-color: var(--color-black);
  margin: 0;
  padding: 0;
}

h1 {
  margin: 0;
}

.header27x41 {
  width: calc(100vw - var(--border-width-double));
  height: calc(100vw * var(--ratio-27x41-header) - var(--border-width-double));
  border-style: ridge;
  border-width: var(--border-width);
  color: var(--color-white);
  background-color: var(--color-black);
  font-family: 'Limelight';
  font-size: 4.6vw;
  display: flex;
  justify-content: center;
  align-items: center;
}

.header27x40 {
  width: calc(100vw - var(--border-width-double));
  height: calc(100vw * var(--ratio-27x40-header) - var(--border-width-double));
  border-style: ridge;
  border-width: var(--border-width);
  color: var(--color-white);
  background-color: var(--color-black);
  font-family: 'Limelight';
  font-size: 4.8vw;
  display: flex;
  justify-content: center;
  align-items: center;
}

.poster27x41 {
  top: calc(100vw * var(--ratio-27x41-header));
  width: 100%;
}

.poster27x40 {
  top: calc(100vw * var(--ratio-27x40-header));
  width: 100%;
}

.poster-img {
  width: 100%;
  margin: auto;
  display: block;
}

.footer27x41 {
  position: fixed;
  top: calc(100vw * var(--ratio-27x41-footer));
  width: calc(100vw - var(--border-width-double));
  height: calc(100vw * var(--ratio-27x41-footer-height) - var(--border-width-double));
  color: var(--color-white);
  background-color: var(--color-black);
  border-style: ridge;
  border-width: var(--border-width);
  display: flex;
  justify-content: center;
  align-items: center;
}

.footer27x40 {
  position: fixed;
  top: calc(100vw * var(--ratio-27x40-footer));
  width: calc(100vw - var(--border-width-double));
  height: calc(100vw * var(--ratio-27x40-footer-height) - var(--border-width-double));
  color: var(--color-white);
  background-color: var(--color-black);
  border-style: ridge;
  border-width: var(--border-width);
  display: flex;
  justify-content: center;
  align-items: center;
}

.footer-img {
  margin: auto 10px;
  height: 80%;
  display: block;
}

/* Pause Indicator */
.pause-indicator {
  position: fixed;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  z-index: 999;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 20px;
  padding: 40px 60px;
  background: rgba(0, 0, 0, 0.85);
  backdrop-filter: blur(10px);
  border-radius: 20px;
  box-shadow: 0 20px 60px rgba(0, 0, 0, 0.5);
  border: 2px solid rgba(255, 255, 255, 0.2);
}

.pause-icon {
  display: flex;
  gap: 12px;
  align-items: center;
}

.pause-bar {
  width: 20px;
  height: 60px;
  background: var(--color-white);
  border-radius: 5px;
  box-shadow: 0 4px 15px rgba(255, 255, 255, 0.3);
}

.pause-text {
  color: var(--color-white);
  font-size: 24px;
  font-weight: 700;
  letter-spacing: 3px;
  text-shadow: 0 2px 10px rgba(0, 0, 0, 0.5);
}

/* Fade transition for pause indicator */
.fade-enter-active,
.fade-leave-active {
  transition: all 0.3s ease;
}

.fade-enter-from,
.fade-leave-to {
  opacity: 0;
  transform: translate(-50%, -50%) scale(0.9);
}

.fade-enter-to,
.fade-leave-from {
  opacity: 1;
  transform: translate(-50%, -50%) scale(1);
}
</style>
