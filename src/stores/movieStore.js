// stores/movieStore.js
import { defineStore } from 'pinia'
import { shuffleArray } from '../util.js'
import moviesData from '../assets/movies.json'

export const useMovieStore = defineStore('movie', {
  state: () => ({
    // Movie data
    allMovies: [],
    randomOrder: [],
    
    // Current state
    movieIndex: 0,
    randomIndex: 0,
    
    // Display settings
    displayMode: 'COMING SOON', // 'COMING SOON' or 'NOW SHOWING'
    displayDuration: 30, // in seconds
    cyclePosters: false,
    
    // UI state
    searchString: '',
    sourceImage: '/brands/Paradigm.png'
  }),

  getters: {
    // Get current movie object
    currentMovie: (state) => {
      return state.allMovies[state.movieIndex] || null
    },

    // Get current movie title
    movieTitle: (state) => {
      const movie = state.allMovies[state.movieIndex]
      return movie ? movie.title : ''
    },

    // Get formatted movie date
    movieDate: (state) => {
      const movie = state.allMovies[state.movieIndex]
      if (!movie) return ''
      return `${movie.month} ${movie.day}, ${movie.year}`
    },

    // Get poster size (27x40 or 27x41)
    posterSize: (state) => {
      const movie = state.allMovies[state.movieIndex]
      return movie ? movie.size : '27x40'
    },

    // Get current poster index
    currentPosterIndex: (state) => {
      const movie = state.allMovies[state.movieIndex]
      return movie ? movie.posterIndex : 0
    },

    // Get poster image path
    posterImage: (state) => {
      const movie = state.allMovies[state.movieIndex]
      if (!movie) return ''
      
      let img = '/movies/'
      img += movie.decade + '/'
      img += movie.year + '/'
      img += movie.folder + '/'
      img += movie.rootname + '-'
      img += movie.posters[movie.posterIndex]
      img += '.jpg'
      return img
    },

    // Get display duration in milliseconds
    displayDurationMs: (state) => {
      return state.displayDuration * 1000
    },

    // Get all posters for current movie
    moviePosters: (state) => {
      const movie = state.allMovies[state.movieIndex]
      if (!movie) return []
      
      return movie.posters.map((poster, index) => ({
        name: poster,
        index: index
      }))
    }
  },

  actions: {
    // Initialize the store with movie data
    initMovies() {
      this.allMovies = JSON.parse(JSON.stringify(moviesData))
      this.randomOrder = []

      this.allMovies.forEach((movie, index) => {
        movie.posterIndex = 0
        this.randomOrder.push(index)
      })

      shuffleArray(this.randomOrder)

      this.displayMode = 'COMING SOON'
      this.randomIndex = 0
      this.movieIndex = this.randomOrder[0]

      this.setDisplayMode('COMING SOON')
      this.gotoMainPoster()
    },

    // Set display mode
    setDisplayMode(mode) {
      this.displayMode = mode
      if (mode === 'COMING SOON') {
        this.sourceImage = '/brands/Paradigm.png'
      }
    },

    // Set display duration (in seconds)
    setDisplayDuration(seconds) {
      this.displayDuration = seconds
    },

    // Toggle cycle posters
    toggleCyclePosters() {
      this.cyclePosters = !this.cyclePosters
    },

    // Set current movie by index
    setCurrentMovie(index) {
      if (index < 0 || index >= this.allMovies.length) return

      this.movieIndex = index

      const newIndexInOrder = this.randomOrder.indexOf(index)
      if (newIndexInOrder !== -1) {
        this.randomIndex = newIndexInOrder
      }
    },

    // Go to next movie in random order
    gotoNextMovie() {
      if (this.randomOrder.length === 0)
        return

      this.randomIndex = (this.randomIndex + 1) % this.randomOrder.length
      this.setCurrentMovie(this.randomOrder[this.randomIndex])
      this.gotoMainPoster()
    },

    // Go to next movie in random order
    gotoPreviousMovie() {
      this.randomIndex--
      if (this.randomIndex < 0) {
        this.randomIndex = this.randomOrder.length - 1
      }
      this.setCurrentMovie(this.randomOrder[this.randomIndex])
      this.gotoMainPoster()
    },

    // Set current poster by index
    setCurrentPoster(index) {
      const movie = this.allMovies[this.movieIndex]
      if (movie && index >= 0 && index < movie.posters.length) {
        movie.posterIndex = index
      }
    },

    // Go to main poster
    gotoMainPoster() {
      const movie = this.allMovies[this.movieIndex]
      if (!movie) return
      movie.posterIndex = 0
    },

    // Go to next poster
    gotoNextPoster() {
      const movie = this.allMovies[this.movieIndex]
      if (!movie) return

      let posterIndex = movie.posterIndex + 1
      const posterCount = movie.posters.length

      if (posterIndex >= posterCount) {
        posterIndex = 0
      }

      movie.posterIndex = posterIndex
    },

    // Go to next poster
    gotoPreviousPoster() {
      const movie = this.allMovies[this.movieIndex]
      if (!movie) return

      let posterIndex = movie.posterIndex - 1
      const posterCount = movie.posters.length

      if (posterIndex < 0) {
        posterIndex = posterCount - 1
      }

      movie.posterIndex = posterIndex
    },

    // Search movies by title
    searchMovies(searchText) {
      if (!searchText) {
        this.searchString = ''
        return []
      }

      this.searchString = searchText
      const results = []
      const lowerSearch = searchText.toLowerCase()

      this.allMovies.forEach((movie, index) => {
        if (movie.title.toLowerCase().includes(lowerSearch)) {
          results.push({
            name: movie.title,
            index: index
          })
        }
      })

      return results
    },

    // Clear search string
    clearSearchString() {
      this.searchString = ''
    },

    // Set source image
    setSourceImage(image) {
      this.sourceImage = image
    }
  }
})
