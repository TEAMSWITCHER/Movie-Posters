<template>
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
<div class="backdrop" @click.self="closeSearch">
    <div class="search" >
        <h2 style="text-align:center;">{{ movieTitle }}</h2>
        <h3 style="text-align:center;">{{ movieDate }}</h3>
        <br>
        <br>

        <div vertical-align="middle" class="search-mode">
        </div>
        <div class="edit">
            <input type="text" v-model="searchInput" :placeholder="getPlaceHolder" @keyup.enter="doSearch">
            <button @click="doSearch"><i class="fa fa-search"></i></button>
        </div>
        <div class="results" v-if="hasResults">
            <ul>
                <li v-for="movie in searchResults" :key="movie.name" @click="setMovie(movie)">
                    <h3>{{ movie.name }}</h3>
                </li>
            </ul>
        </div>
    </div>
</div>
</template>

<script>
import { ref, computed, onMounted, onBeforeUnmount } from 'vue'
import { useMovieStore } from '../stores/movieStore.js'

export default {
  name: 'Search',
  emits: ['close'],
  setup(props, { emit }) {
    const movieStore = useMovieStore()

    // Local state
    const searchInput = ref('')
    const searchResults = ref([])

    // Computed from store
    const movieTitle = computed(() => movieStore.movieTitle)
    const movieDate = computed(() => movieStore.movieDate)
    const searchString = computed(() => movieStore.searchString)
    const hasResults = computed(() => searchResults.value.length > 0)

    // Computed placeholder
    const placeholder = computed(() => 
      searchString.value !== '' ? searchString.value : 'Search...'
    )

    // Methods
    const doSearch = () => {
      const trimmedSearch = searchInput.value.trim()
      
      if (trimmedSearch !== '') {
        searchResults.value = movieStore.searchMovies(trimmedSearch)
      } else {
        searchResults.value = []
        movieStore.clearSearchString()
      }
    }

    const setMovie = (movie) => {
      movieStore.setCurrentMovie(movie.index)
      movieStore.setCurrentPoster(0)
      movieStore.setDisplayMode('NOW SHOWING')
      emit('close')
    }

    const closeSearch = () => {
      emit('close')
    }

    const handleKeydown = (event) => {
      if (event.key === 'Escape') {
        closeSearch()
      }
    }

    // Initialize on mount
    onMounted(() => {
      const lastSearch = movieStore.searchString
      searchInput.value = lastSearch
      
      if (lastSearch !== '') {
        searchResults.value = movieStore.searchMovies(lastSearch)
      }

      window.addEventListener('keydown', handleKeydown)
    })

    onBeforeUnmount(() => {
      window.removeEventListener('keydown', handleKeydown)
    })

    return {
      // State
      searchInput,
      searchResults,
      movieTitle,
      movieDate,
      searchString,
      hasResults,
      
      // Computed
      getPlaceHolder: placeholder,
      
      // Methods
      doSearch,
      setMovie,
      closeSearch
    }
  }
}
</script>

<style scoped>
:root {
  /* Color Variables - Complementary palette to Settings */
  --modal-bg: linear-gradient(135deg, #f093fb 0%, #f5576c 100%);
  --modal-overlay: rgba(0, 0, 0, 0.85);
  --color-white: #FFFFFF;
  --color-primary: #f5576c;
  --color-primary-hover: #f093fb;
  --input-bg: rgba(255, 255, 255, 0.15);
  --input-border: rgba(255, 255, 255, 0.3);
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

div.search {
  display: flex;
  flex-direction: column;
  width: 620px;
  max-height: 90vh;
  padding: 30px;
  background: var(--modal-bg);
  border-radius: 20px;
  box-shadow: var(--shadow-lg);
  color: var(--text-color);
  animation: slideUp 0.4s cubic-bezier(0.16, 1, 0.3, 1);
  overflow: hidden;
}

div.search h2 {
  color: white;  
  margin: 0 0 5px 0;
  font-size: 28px;
  font-weight: 700;
  text-shadow: 0 2px 10px rgba(0, 0, 0, 0.3);
}

div.search h3 {
  color: white;
  margin: 0 0 20px 0;
  font-size: 16px;
  opacity: 0.9;
  font-weight: 400;
}

div.search-mode {
  margin: 10px 0;
}

div.edit {
  display: flex;
  gap: 10px;
  margin: 10px 0 20px 0;
}

div.edit input[type=text] {
  flex: 1;
  padding: 14px 20px;
  font-size: 16px;
  border: 2px solid var(--input-border);
  background: var(--input-bg);
  backdrop-filter: blur(10px);
  color: var(--color-white);
  border-radius: 12px;
  transition: all 0.3s ease;
  box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
}

div.edit input[type=text]:focus {
  outline: none;
  background: rgba(255, 255, 255, 0.2);
  border-color: var(--color-white);
  box-shadow: 0 0 0 3px rgba(255, 255, 255, 0.2), 0 4px 20px rgba(0, 0, 0, 0.3);
}

div.edit input[type=text]::placeholder {
  color: rgba(255, 255, 255, 0.6);
}

div.edit button {
  width: 50px;
  padding: 14px;
  background: var(--color-white);
  color: var(--color-primary);
  font-size: 18px;
  border: none;
  border-radius: 12px;
  cursor: pointer;
  transition: all 0.3s ease;
  box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
  display: flex;
  align-items: center;
  justify-content: center;
}

div.edit button:hover {
  background: var(--color-primary);
  color: var(--color-white);
  transform: translateY(-2px);
  box-shadow: 0 6px 20px rgba(0, 0, 0, 0.4);
}

div.edit button:active {
  transform: translateY(0);
}

div.results {
  flex: 1;
  width: 100%;
  margin: 0;
  overflow-y: auto;
  padding-right: 5px;
}

div.results ul {
  padding: 0;
  margin: 0;
  list-style: none;
}

div.results li {
  background: rgba(255, 255, 255, 0.15);
  backdrop-filter: blur(10px);
  margin: 0 0 10px 0;
  padding: 0;
  border: 2px solid rgba(255, 255, 255, 0.2);
  border-radius: 12px;
  transition: all 0.3s ease;
  cursor: pointer;
  overflow: hidden;
}

div.results li:hover {
  background: rgba(255, 255, 255, 0.25);
  border-color: var(--color-white);
  transform: translateX(5px);
  box-shadow: 0 5px 20px rgba(0, 0, 0, 0.3);
}

div.results li:active {
  transform: translateX(3px) scale(0.98);
}

div.results h3 {
  margin: 0;
  padding: 16px 20px;
  cursor: pointer;
  font-size: 16px;
  font-weight: 600;
  letter-spacing: 0.3px;
}

/* Scrollbar Styling */
div.results::-webkit-scrollbar {
  width: 8px;
}

div.results::-webkit-scrollbar-track {
  background: rgba(255, 255, 255, 0.1);
  border-radius: 10px;
}

div.results::-webkit-scrollbar-thumb {
  background: rgba(255, 255, 255, 0.3);
  border-radius: 10px;
}

div.results::-webkit-scrollbar-thumb:hover {
  background: rgba(255, 255, 255, 0.5);
}

/* Empty state */
div.search:not(:has(.results)) {
  min-height: 300px;
}
</style>