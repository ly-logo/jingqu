<template>
  <div ref="chartRef" class="heat-map"></div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted, watch } from 'vue'
import * as echarts from 'echarts'
import type { Spot } from '@/api'
import { getLevelColor, getLevelLabel } from '@/utils/level'

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
  const data = props.spots.map((s) => ({
    name: s.name,
    value: [s.locationX, s.locationY, s.currentVisitors],
    itemStyle: { color: getLevelColor(s.level) },
    levelLabel: getLevelLabel(s.level),
    capacity: s.capacity,
  }))
  chart.setOption({
    grid: { top: 24, right: 24, bottom: 24, left: 24 },
    xAxis: { type: 'value', show: false, min: 0, max: 100 },
    yAxis: { type: 'value', show: false, min: 0, max: 100 },
    tooltip: {
      formatter: (params: any) => {
        const d = params.data
        return `${d.name}<br/>在园：${d.value[2]} / 容量：${d.capacity}<br/>等级：${d.levelLabel}`
      },
    },
    series: [
      {
        type: 'scatter',
        symbolSize: (val: number[]) => {
          const visitors = val[2]
          return Math.max(10, Math.min(50, visitors / 20))
        },
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
.heat-map {
  width: 100%;
  height: 320px;
}
</style>
