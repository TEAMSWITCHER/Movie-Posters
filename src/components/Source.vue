<template>
<div class="backdrop" @click.self="closeSources">
    <div class="sources">
        <h1>Select Source</h1>
        <ul>
            <li v-for="source in sources" :key="source.img" @click="setImage(source.img)">
                <img :src="source.img" alt="">
            </li>
        </ul>
    </div>
</div>
</template>

<script>
import { onMounted, onBeforeUnmount } from 'vue'
import { useMovieStore } from '../stores/movieStore.js'

export default {
  name: 'Sources',
  emits: ['close'],
  setup(props, { emit }) {
    const movieStore = useMovieStore()

    const sources = [
      { img: 'sources/ultra-hd_blu-ray.png' },
      { img: 'sources/blue-ray-disk.png' },
      { img: 'sources/dvd-video.png' },
      { img: 'sources/apple-tv.png' },
      { img: 'sources/vudu.png' },
      { img: 'sources/hbo.png' },
      { img: 'sources/hbo-max.png' },
      { img: 'sources/disney-plus.png' },
      { img: 'sources/hulu.png' },
      { img: 'sources/netflix.png' },
      { img: 'sources/prime-video.png' },
      { img: 'sources/starz.png' }
    ]

    const setImage = (image) => {
      movieStore.setSourceImage(image)
    }

    const closeSources = () => {
      emit('close')
    }

    const handleKeydown = (event) => {
      if (event.key === 'Escape') {
        closeSources()
      }
    }

    onMounted(() => {
      window.addEventListener('keydown', handleKeydown)
    })

    onBeforeUnmount(() => {
      window.removeEventListener('keydown', handleKeydown)
    })

    return {
      sources,
      setImage,
      closeSources
    }
  }
}
</script>

<style scoped>
:root {
  /* Color Variables - Third complementary palette */
  --modal-bg: linear-gradient(135deg, #4facfe 0%, #00f2fe 100%);
  --modal-overlay: rgba(0, 0, 0, 0.85);
  --color-white: #FFFFFF;
  --color-primary: #4facfe;
  --text-color: #FFFFFF;
  --shadow-lg: 0 20px 60px rgba(0, 0, 0, 0.5);
}

div.backdrop {
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
    transform: translate(-50%, -45%);
  }
  to {
    opacity: 1;
    transform: translate(-50%, -50%);
  }
}

div.sources {
  position: absolute;
  width: 580px;
  max-height: 90vh;
  left: 50%;
  top: 50%;
  transform: translate(-50%, -50%);
  padding: 30px;
  background: var(--modal-bg);
  border-radius: 20px;
  box-shadow: var(--shadow-lg);
  color: var(--text-color);
  animation: slideUp 0.4s cubic-bezier(0.16, 1, 0.3, 1);
  overflow-y: auto;
}

div.sources h1 {
  color: white;
  margin: 0 0 30px 0;
  font-size: 32px;
  font-weight: 700;
  text-align: center;
  text-shadow: 0 2px 10px rgba(0, 0, 0, 0.3);
  letter-spacing: 1px;
}

div.sources ul {
  padding: 0;
  margin: 0;
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(250px, 1fr));
  gap: 15px;
  list-style: none;
}

div.sources li {
  background: rgba(255, 255, 255, 0.15);
  backdrop-filter: blur(10px);
  padding: 20px;
  border: 2px solid rgba(255, 255, 255, 0.2);
  border-radius: 15px;
  display: flex;
  justify-content: center;
  align-items: center;
  cursor: pointer;
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
  min-height: 80px;
  box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
}

div.sources li:hover {
  background: rgba(255, 255, 255, 0.25);
  border-color: var(--color-white);
  transform: translateY(-5px) scale(1.02);
  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.4);
}

div.sources li:active {
  transform: translateY(-2px) scale(1);
}

div.sources img {
  max-height: 60px;
  max-width: 100%;
  object-fit: contain;
  filter: drop-shadow(0 2px 8px rgba(0, 0, 0, 0.3));
  transition: all 0.3s ease;
}

div.sources li:hover img {
  filter: drop-shadow(0 4px 12px rgba(0, 0, 0, 0.5));
  transform: scale(1.05);
}

/* Scrollbar Styling */
div.sources::-webkit-scrollbar {
  width: 8px;
}

div.sources::-webkit-scrollbar-track {
  background: rgba(255, 255, 255, 0.1);
  border-radius: 10px;
}

div.sources::-webkit-scrollbar-thumb {
  background: rgba(255, 255, 255, 0.3);
  border-radius: 10px;
}

div.sources::-webkit-scrollbar-thumb:hover {
  background: rgba(255, 255, 255, 0.5);
}

/* Responsive adjustments */
@media (max-width: 650px) {
  div.sources {
    width: 90%;
  }
  
  div.sources ul {
    grid-template-columns: 1fr;
  }
}
</style>