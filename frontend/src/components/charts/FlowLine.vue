<template>
  <div ref="chartRef" class="flow-line"></div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted, watch } from 'vue'
import * as echarts from 'echarts'

const props = defineProps<{
  data: number[]
  xData?: string[]
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
  const len = props.data.length
  const x = props.xData ?? Array.from({ length: len }, (_, i) => String(i))
  chart.setOption({
    grid: { top: 2, right: 2, bottom: 2, left: 2 },
    xAxis: { type: 'category', data: x, show: false },
    yAxis: { type: 'value', show: false },
    tooltip: { trigger: 'axis' },
    series: [
      {
        type: 'line',
        data: props.data,
        smooth: true,
        showSymbol: false,
        lineStyle: { width: 2, color: '#0d9488' },
        areaStyle: {
          color: new (echarts as any).graphic.LinearGradient(0, 0, 0, 1, [
            { offset: 0, color: 'rgba(20,184,166,0.32)' },
            { offset: 1, color: 'rgba(20,184,166,0.02)' },
          ]),
        },
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

watch(() => [...props.data], updateChart, { deep: true })
</script>

<style scoped>
.flow-line {
  width: 100%;
  height: 60px;
}
</style>
