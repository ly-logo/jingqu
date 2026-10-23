<template>
  <div ref="chartRef" class="forecast-bar"></div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted, watch } from 'vue'
import * as echarts from 'echarts'
import type { ForecastPoint } from '@/api'

// 当日客流时段预测柱状图：高峰区间 markArea 标注（用户端唯一保留的统计图形之一）
const props = defineProps<{
  points: ForecastPoint[]
  peakStart: number
  peakEnd: number
}>()

const chartRef = ref<HTMLDivElement>()
let chart: echarts.ECharts | null = null

function updateChart() {
  if (!chart) return
  const hours = props.points.map((p) => `${p.hour}`)
  const values = props.points.map((p) => p.predictedVisitors)
  chart.setOption({
    grid: { top: 24, right: 8, bottom: 24, left: 48 },
    tooltip: {
      trigger: 'axis',
      formatter: (params: any) => {
        const p = params[0]
        const point = props.points[p.dataIndex]
        return `${point.hour}:00<br/>预测 ${point.predictedVisitors} 人 · ${point.levelText}`
      },
    },
    xAxis: {
      type: 'category',
      data: hours,
      axisLabel: {
        interval: 3,
        formatter: (v: string) => `${v}时`,
      },
      axisTick: { show: false },
    },
    yAxis: { type: 'value', splitLine: { lineStyle: { color: '#f0f0f0' } } },
    series: [
      {
        type: 'bar',
        data: values,
        barMaxWidth: 14,
        itemStyle: { color: '#409eff', borderRadius: [3, 3, 0, 0] },
        markArea: {
          silent: true,
          itemStyle: { color: 'rgba(230,162,60,0.15)' },
          label: { show: true, position: 'insideTop', color: '#e6a23c', fontSize: 11 },
          data: [[{ name: '高峰时段', xAxis: String(props.peakStart) }, { xAxis: String(props.peakEnd) }]],
        },
      },
    ],
  })
}

function handleResize() {
  chart?.resize()
}

onMounted(() => {
  if (!chartRef.value) return
  chart = echarts.init(chartRef.value)
  updateChart()
  window.addEventListener('resize', handleResize)
})

onUnmounted(() => {
  window.removeEventListener('resize', handleResize)
  chart?.dispose()
  chart = null
})

watch(() => props.points, updateChart, { deep: true })
watch(() => [props.peakStart, props.peakEnd], updateChart)
</script>

<style scoped>
.forecast-bar {
  width: 100%;
  height: 200px;
}
</style>
