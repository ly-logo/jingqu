<template>
  <div ref="chartRef" class="level-pie"></div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted, watch } from 'vue'
import * as echarts from 'echarts'
import type { Spot } from '@/api'
import { LEVEL_COLORS, getLevelLabel } from '@/utils/level'

const props = defineProps<{
  spots: Spot[]
}>()

const chartRef = ref<HTMLDivElement>()
let chart: echarts.ECharts | null = null

function initChart() {
  if (!chartRef.value) return
  chart = echarts.init(chartRef.value)
  updateChart()
  window.addEventListener('resize', handleResize)
}

function updateChart() {
  if (!chart) return
  const counts: Record<string, number> = {}
  for (const s of props.spots) {
    counts[s.level] = (counts[s.level] || 0) + 1
  }
  const data = Object.entries(counts).map(([level, count]) => ({
    name: getLevelLabel(level as any),
    value: count,
    itemStyle: { color: LEVEL_COLORS[level as keyof typeof LEVEL_COLORS] || '#909399' },
  }))
  chart.setOption({
    tooltip: { trigger: 'item' },
    legend: { bottom: 0, itemWidth: 10, itemHeight: 10, textStyle: { fontSize: 12 } },
    series: [
      {
        type: 'pie',
        radius: ['40%', '65%'],
        center: ['50%', '45%'],
        avoidLabelOverlap: false,
        label: { show: true, formatter: '{b}\n{d}%' },
        data,
      },
    ],
  })
}

function handleResize() {
  chart?.resize()
}

onMounted(initChart)
onUnmounted(() => {
  window.removeEventListener('resize', handleResize)
  chart?.dispose()
  chart = null
})

watch(() => [...props.spots], updateChart, { deep: true })
</script>

<style scoped>
.level-pie {
  width: 100%;
  height: 320px;
}
</style>
