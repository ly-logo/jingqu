<template>
  <div class="statistics-page">
    <el-card shadow="never" class="filter-card">
      <div class="filter-bar">
        <span class="filter-label">时间范围</span>
        <el-radio-group v-model="range" @change="onRangeChange">
          <el-radio-button value="today">今日</el-radio-button>
          <el-radio-button value="7d">近 7 天</el-radio-button>
          <el-radio-button value="custom">自定义</el-radio-button>
        </el-radio-group>
        <el-date-picker
          v-if="range === 'custom'"
          v-model="customRange"
          type="daterange"
          range-separator="至"
          start-placeholder="开始日期"
          end-placeholder="结束日期"
          value-format="YYYY-MM-DD"
          size="default"
          @change="fetchData"
        />
        <span class="refresh-tip">每 30 秒自动刷新</span>
      </div>
    </el-card>

    <div class="chart-grid">
      <el-card shadow="never" class="chart-card">
        <template #header>
          <div class="card-header">流量趋势</div>
        </template>
        <div ref="flowRef" class="chart"></div>
      </el-card>

      <el-card shadow="never" class="chart-card">
        <template #header>
          <div class="card-header">各景点拥堵时长占比</div>
        </template>
        <div ref="congestionRef" class="chart"></div>
      </el-card>

      <el-card shadow="never" class="chart-card">
        <template #header>
          <div class="card-header">停车位利用率曲线</div>
        </template>
        <div ref="resourceRef" class="chart"></div>
      </el-card>

      <el-card shadow="never" class="chart-card">
        <template #header>
          <div class="card-header">调度建议类型分布</div>
        </template>
        <div ref="suggestionRef" class="chart"></div>
      </el-card>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted } from 'vue'
import * as echarts from 'echarts'
import { getStatistics } from '@/api'
import type { StatisticsData } from '@/api'
import { SUGGESTION_TYPE_LABELS } from '@/utils/status'

const range = ref('7d')
const customRange = ref<[string, string] | null>(null)
const data = ref<StatisticsData | null>(null)

const flowRef = ref<HTMLDivElement>()
const congestionRef = ref<HTMLDivElement>()
const resourceRef = ref<HTMLDivElement>()
const suggestionRef = ref<HTMLDivElement>()

let flowChart: echarts.ECharts | null = null
let congestionChart: echarts.ECharts | null = null
let resourceChart: echarts.ECharts | null = null
let suggestionChart: echarts.ECharts | null = null

const SUGGESTION_COLORS = ['#409eff', '#67C23A', '#E6A23C', '#FF7A45', '#F56C6C']

async function fetchData() {
  try {
    const res = await getStatistics(range.value)
    data.value = res.data
    renderCharts()
  } catch (e) {}
}

function onRangeChange() {
  if (range.value === 'custom') return
  fetchData()
}

function renderCharts() {
  if (!data.value) return
  const d = data.value

  flowChart?.setOption({
    grid: { top: 32, right: 24, bottom: 32, left: 56 },
    tooltip: { trigger: 'axis' },
    xAxis: { type: 'category', data: d.flow.map((f) => f.date), boundaryGap: false },
    yAxis: { type: 'value', name: '入园人数' },
    series: [
      {
        type: 'line',
        smooth: true,
        showSymbol: false,
        data: d.flow.map((f) => f.visitors),
        lineStyle: { width: 2, color: '#409eff' },
        areaStyle: {
          color: new (echarts as any).graphic.LinearGradient(0, 0, 0, 1, [
            { offset: 0, color: 'rgba(64,158,255,0.35)' },
            { offset: 1, color: 'rgba(64,158,255,0.02)' },
          ]),
        },
      },
    ],
  })

  congestionChart?.setOption({
    grid: { top: 24, right: 24, bottom: 48, left: 56 },
    tooltip: { trigger: 'axis', axisPointer: { type: 'shadow' } },
    xAxis: {
      type: 'category',
      data: d.congestion.map((c) => c.spotName),
      axisLabel: { interval: 0, rotate: 30, fontSize: 11 },
    },
    yAxis: { type: 'value', name: '拥堵时长(分)' },
    series: [
      {
        type: 'bar',
        barMaxWidth: 28,
        data: d.congestion.map((c) => ({
          value: c.crowdedMinutes,
          itemStyle: {
            color:
              c.crowdedMinutes > 260 ? '#F56C6C' : c.crowdedMinutes > 180 ? '#FF7A45' : '#E6A23C',
          },
        })),
      },
    ],
  })

  resourceChart?.setOption({
    grid: { top: 32, right: 24, bottom: 32, left: 56 },
    tooltip: { trigger: 'axis', valueFormatter: (v: number) => `${v}%` },
    xAxis: { type: 'category', data: d.resource.map((r) => r.time), boundaryGap: false },
    yAxis: { type: 'value', name: '利用率(%)', max: 100 },
    series: [
      {
        type: 'line',
        smooth: true,
        showSymbol: false,
        data: d.resource.map((r) => r.utilization),
        lineStyle: { width: 2, color: '#67C23A' },
        itemStyle: { color: '#67C23A' },
      },
    ],
  })

  suggestionChart?.setOption({
    tooltip: { trigger: 'item', formatter: '{b}: {c} ({d}%)' },
    legend: { bottom: 0, itemWidth: 10, itemHeight: 10, textStyle: { fontSize: 12 } },
    series: [
      {
        type: 'pie',
        radius: ['40%', '68%'],
        center: ['50%', '45%'],
        label: { show: true, formatter: '{b}\n{d}%' },
        data: d.suggestionTypeCount.map((s, i) => ({
          name: SUGGESTION_TYPE_LABELS[s.type] || s.type,
          value: s.count,
          itemStyle: { color: SUGGESTION_COLORS[i % SUGGESTION_COLORS.length] },
        })),
      },
    ],
  })
}

function handleResize() {
  flowChart?.resize()
  congestionChart?.resize()
  resourceChart?.resize()
  suggestionChart?.resize()
}

onMounted(async () => {
  if (flowRef.value) flowChart = echarts.init(flowRef.value)
  if (congestionRef.value) congestionChart = echarts.init(congestionRef.value)
  if (resourceRef.value) resourceChart = echarts.init(resourceRef.value)
  if (suggestionRef.value) suggestionChart = echarts.init(suggestionRef.value)
  window.addEventListener('resize', handleResize)
  await fetchData()
})

onUnmounted(() => {
  window.removeEventListener('resize', handleResize)
  flowChart?.dispose()
  congestionChart?.dispose()
  resourceChart?.dispose()
  suggestionChart?.dispose()
})
</script>

<style scoped>
.statistics-page {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.filter-card {
  border-radius: 6px;
}

.filter-bar {
  display: flex;
  align-items: center;
  gap: 16px;
}

.filter-label {
  font-size: 14px;
  color: #606266;
}

.refresh-tip {
  margin-left: auto;
  font-size: 12px;
  color: #909399;
}

.chart-grid {
  display: grid;
  grid-template-columns: repeat(2, 1fr);
  gap: 16px;
}

.chart-card {
  border-radius: 6px;
}

.card-header {
  font-size: 15px;
  font-weight: 600;
  color: #303133;
}

.chart {
  width: 100%;
  height: 300px;
}

@media (max-width: 1100px) {
  .chart-grid {
    grid-template-columns: 1fr;
  }
}
</style>