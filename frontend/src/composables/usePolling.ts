import { onMounted, onUnmounted } from 'vue'

export function usePolling(fn: () => void | Promise<void>, interval = 3000) {
  let timer: ReturnType<typeof setInterval> | null = null

  function start() {
    if (timer) return
    timer = setInterval(() => {
      if (!document.hidden) {
        fn()
      }
    }, interval)
  }

  function stop() {
    if (timer) {
      clearInterval(timer)
      timer = null
    }
  }

  function onVisibility() {
    if (!document.hidden) {
      fn()
    }
  }

  onMounted(() => {
    fn()
    start()
    document.addEventListener('visibilitychange', onVisibility)
  })

  onUnmounted(() => {
    stop()
    document.removeEventListener('visibilitychange', onVisibility)
  })

  return { start, stop }
}
