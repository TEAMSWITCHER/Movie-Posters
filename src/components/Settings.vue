<template>
<div class="backdrop" @click.self="closeSettings">
    <div class="settings">
        <div>
            <h2 style="text-align:center;">{{ movieTitle }}</h2> 
            <h3 style="text-align:center;">{{ movieDate }}</h3>
        </div>
        <div vertical-align="middle" class="display-mode">
            <label align="left" >Display Mode:</label>
            <div class="mode">
                <button :class="{ active: isComingSoon }" @click="onClickComingSoon">COMING SOON</button>
                <button :class="{ active: isNowShowing }" @click="onClickNowShowing">NOW SHOWING</button>
            </div>
        </div>
        <div class="cycle-posters" v-if="isNowShowing">
            <label align="left">Cycle all posters for this movie:</label>
            <label class="switch">
                <input type="checkbox" :checked="doCyclePosters" @change="cyclePosters($event)">
                <span class="slider round"></span>
            </label>
        </div>
        <div class="display-duration" v-if="showPosterDuration">
            <label align="left">Display Duration (1 to 60 Seconds):</label>
            <div class="input">
                <input type="number" id="duration" min="1" max="60" :placeholder="displayDuration" @change="setDuration($event)">
            </div>
        </div>
        <div class="current-poster" v-if="isNowShowing">
            <label align="left">Current Poster:</label>
            <select class="selector" float="right" name="posters" id="poster" @change="setPoster($event)">
                <option v-for="poster in posters" :key="poster.name" :selected="(poster.index == posterIndex)">{{ poster.name }}</option>
            </select>
        </div>
        <div v-if="isNowShowing">
            <!-- List all posters here -->
        </div>
    </div>
</div>
</template>

<script>
import { computed, onMounted, onBeforeUnmount } from 'vue'
import { useMovieStore } from '../stores/movieStore.js'

export default {
  name: 'Settings',
  emits: ['close'],
  setup(props, { emit }) {
    const movieStore = useMovieStore()

    // Computed properties from store
    const displayMode = computed(() => movieStore.displayMode)
    const displayDuration = computed(() => movieStore.displayDuration)
    const movieTitle = computed(() => movieStore.movieTitle)
    const movieDate = computed(() => movieStore.movieDate)
    const posterIndex = computed(() => movieStore.currentPosterIndex)
    const posters = computed(() => movieStore.moviePosters)
    const cyclePostersEnabled = computed(() => movieStore.cyclePosters)

    // Helper methods
    const isComingSoon = computed(() => displayMode.value === 'COMING SOON')
    const isNowShowing = computed(() => displayMode.value === 'NOW SHOWING')
    const showPosterDuration = computed(() => 
      isComingSoon.value || cyclePostersEnabled.value
    )

    // Actions
    const onClickComingSoon = () => {
      movieStore.setDisplayMode('COMING SOON')
    }

    const onClickNowShowing = () => {
      movieStore.setDisplayMode('NOW SHOWING')
    }

    const setDuration = (event) => {
      const newDuration = parseInt(event.target.value)
      if (!isNaN(newDuration) && newDuration !== displayDuration.value) {
        movieStore.setDisplayDuration(newDuration)
      }
    }

    const handleCyclePosters = () => {
      movieStore.toggleCyclePosters()
    }

    const setPoster = (event) => {
      const selectedPoster = event.target.value
      const index = posters.value.findIndex(poster => poster.name === selectedPoster)
      if (index !== -1) {
        movieStore.setCurrentPoster(index)
      }
    }

    const closeSettings = () => {
      emit('close')
    }

    const handleKeydown = (event) => {
      if (event.key === 'Escape') {
        closeSettings()
      }
    }

    onMounted(() => {
      window.addEventListener('keydown', handleKeydown)
    })

    onBeforeUnmount(() => {
      window.removeEventListener('keydown', handleKeydown)
    })

    return {
      // Computed state
      displayMode,
      displayDuration,
      movieTitle,
      movieDate,
      posterIndex,
      posters,
      
      // Computed helpers
      isComingSoon,
      isNowShowing,
      showPosterDuration,
      doCyclePosters: cyclePostersEnabled,
      
      // Methods
      onClickComingSoon,
      onClickNowShowing,
      setDuration,
      cyclePosters: handleCyclePosters,
      setPoster,
      closeSettings
    }
  }
}
</script>

<style scoped>
:root {
  /* Color Variables - Modern Palette */
  --modal-bg: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  --modal-overlay: rgba(0, 0, 0, 0.85);
  --color-white: #FFFFFF;
  --color-primary: #667eea;
  --color-primary-hover: #764ba2;
  --color-accent: #f093fb;
  --input-bg: rgba(255, 255, 255, 0.1);
  --input-border: rgba(255, 255, 255, 0.2);
  --text-color: #FFFFFF;
  --shadow-lg: 0 20px 60px rgba(0, 0, 0, 0.5);
}

.backdrop {
  top: 0;
  position: fixed;
  background: var(--modal-overlay);
  backdrop-filter: blur(10px);
  width: 100%;
  height: 100%;
  display: flex;
  align-items: center;
  justify-content: center;
  z-index: 1000;
  animation: fadeIn 0.3s ease;
}

@keyframes fadeIn {
  from { opacity: 0; }
  to { opacity: 1; }
}

@keyframes slideUp {
  from {
    opacity: 0;
    transform: translateY(30px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

.settings {
  width: 560px;
  max-height: 90vh;
  padding: 30px;
  background: var(--modal-bg);
  border-radius: 20px;
  box-shadow: var(--shadow-lg);
  color: var(--text-color);
  animation: slideUp 0.4s cubic-bezier(0.16, 1, 0.3, 1);
  overflow-y: auto;
}

.settings h2 {
  color: white;
  margin: 0 0 5px 0;
  font-size: 28px;
  font-weight: 700;
  text-shadow: 0 2px 10px rgba(0, 0, 0, 0.3);
}

.settings h3 {
  color: white;
  margin: 0 0 25px 0;
  font-size: 16px;
  opacity: 0.9;
  font-weight: 400;
}

.display-mode,
.display-type,
.cycle-posters,
.display-duration,
.current-poster {
  color: white;
  margin: 20px 0;
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 15px;
  background: rgba(255, 255, 255, 0.1);
  border-radius: 12px;
  backdrop-filter: blur(10px);
  transition: all 0.3s ease;
}

.display-mode:hover,
.display-type:hover,
.cycle-posters:hover,
.display-duration:hover,
.current-poster:hover {
  background: rgba(255, 255, 255, 0.15);
  transform: translateX(5px);
}

.display-mode label,
.display-type label,
.cycle-posters label:first-child,
.display-duration label,
.current-poster label {
  font-weight: 600;
  font-size: 14px;
  letter-spacing: 0.5px;
  text-transform: uppercase;
  flex: 1;
}

.mode {
  display: flex;
  gap: 10px;
  border-radius: 10px;
  overflow: hidden;
  box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
}

.mode button {
  color: var(--color-white);
  background: rgba(255, 255, 255, 0.2);
  border: 2px solid rgba(255, 255, 255, 0.3);
  width: 125px;
  padding: 12px 16px;
  cursor: pointer;
  font-size: 13px;
  font-weight: 600;
  letter-spacing: 0.5px;
  transition: all 0.3s ease;
  border-radius: 8px;
}

.mode button:hover {
  background: rgba(255, 255, 255, 0.3);
  transform: translateY(-2px);
  box-shadow: 0 5px 15px rgba(0, 0, 0, 0.3);
}

.mode button.active {
  background: var(--color-white);
  color: var(--color-primary);
  border-color: var(--color-white);
  box-shadow: 0 5px 20px rgba(255, 255, 255, 0.4);
}

.input,
.selector {
  width: 250px;
  padding: 10px 15px;
  background: rgba(255, 255, 255, 0.2);
  border: 2px solid rgba(255, 255, 255, 0.3);
  border-radius: 10px;
  color: var(--color-white);
  font-size: 14px;
  transition: all 0.3s ease;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
}

.input:focus,
.selector:focus {
  outline: none;
  background: rgba(255, 255, 255, 0.25);
  border-color: var(--color-white);
  box-shadow: 0 0 0 3px rgba(255, 255, 255, 0.2);
}

.input input[type="number"] {
  width: 100%;
  background: transparent;
  border: none;
  color: var(--color-white);
  font-size: 14px;
}

.input input[type="number"]:focus {
  outline: none;
}

.input input[type="number"]::placeholder {
  color: rgba(255, 255, 255, 0.6);
}

.selector {
  cursor: pointer;
}

.selector option {
  background: var(--color-primary);
  color: var(--color-white);
}

/* Toggle Switch Styling */
.switch {
  position: relative;
  display: inline-block;
  width: 60px;
  height: 34px;
}

.switch input {
  opacity: 0;
  width: 0;
  height: 0;
}

.slider {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background: rgba(255, 255, 255, 0.2);
  border: 2px solid rgba(255, 255, 255, 0.3);
  transition: all 0.4s cubic-bezier(0.4, 0, 0.2, 1);
  box-shadow: inset 0 2px 5px rgba(0, 0, 0, 0.2);
}

.slider:before {
  position: absolute;
  content: "";
  height: 24px;
  width: 24px;
  left: 3px;
  bottom: 3px;
  background: var(--color-white);
  transition: all 0.4s cubic-bezier(0.4, 0, 0.2, 1);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.3);
}

input:hover + .slider {
  background: rgba(255, 255, 255, 0.3);
}

input:checked + .slider {
  background: var(--color-white);
  border-color: var(--color-white);
}

input:checked + .slider:before {
  background: var(--color-primary);
  transform: translateX(26px);
  box-shadow: 0 2px 12px rgba(102, 126, 234, 0.6);
}

input:focus + .slider {
  box-shadow: 0 0 0 3px rgba(255, 255, 255, 0.3);
}

.slider.round {
  border-radius: 34px;
}

.slider.round:before {
  border-radius: 50%;
}

/* Scrollbar Styling */
.settings::-webkit-scrollbar {
  width: 8px;
}

.settings::-webkit-scrollbar-track {
  background: rgba(255, 255, 255, 0.1);
  border-radius: 10px;
}

.settings::-webkit-scrollbar-thumb {
  background: rgba(255, 255, 255, 0.3);
  border-radius: 10px;
}

.settings::-webkit-scrollbar-thumb:hover {
  background: rgba(255, 255, 255, 0.5);
}
</style>