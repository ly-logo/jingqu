<template>
  <div ref="chartRef" class="capacity-ring"></div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted, watch, computed } from 'vue'
import * as echarts from 'echarts'
import { ratioToLevel, getLevelColor } from '@/utils/level'

const props = defineProps<{
  current: number
  capacity: number
}>()

const chartRef = ref<HTMLDivElement>()
let chart: echarts.ECharts | null = null

const ratio = computed(() => props.capacity > 0 ? props.current / props.capacity : 0)
const color = computed(() => getLevelColor(ratioToLevel(ratio.value)))

function initChart() {
  if (!chartRef.value) return
  chart = echarts.init(chartRef.value)
  updateChart()
  window.addEventListener('resize', handleResize)
}

function updateChart() {
  if (!chart) return
  const value = Math.min(ratio.value, 1.15)
  chart.setOption({
    series: [
      {
        type: 'pie',
        radius: ['60%', '75%'],
        center: ['50%', '50%'],
        avoidLabelOverlap: false,
        label: { show: false },
        emphasis: { scale: false },
        data: [
          {
            value: value,
            itemStyle: { color: color.value },
          },
          {
            value: Math.max(0, 1 - value),
            itemStyle: { color: '#ebeef5' },
          },
        ],
      },
    ],
    graphic: [
      {
        type: 'text',
        left: 'center',
        top: '40%',
        style: {
          text: `${props.current}/${props.capacity}`,
          fontSize: 14,
          fontWeight: 600,
          fill: '#303133',
        },
      },
      {
        type: 'text',
        left: 'center',
        top: '55%',
        style: {
          text: `${(ratio.value * 100).toFixed(0)}%`,
          fontSize: 12,
          fill: '#909399',
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

watch(() => [props.current, props.capacity], updateChart, { deep: true })
</script>

<style scoped>
.capacity-ring {
  width: 100%;
  height: 120px;
}
</style>
