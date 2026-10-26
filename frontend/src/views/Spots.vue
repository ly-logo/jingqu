<template>
  <div class="spots-page">
    <div class="toolbar">
      <el-input
        v-model="search"
        placeholder="搜索景点"
        clearable
        style="width: 240px"
      />
      <el-select v-model="filterLevel" placeholder="等级筛选" clearable style="width: 160px">
        <el-option label="空闲" value="EMPTY" />
        <el-option label="正常" value="NORMAL" />
        <el-option label="适中" value="MODERATE" />
        <el-option label="拥挤" value="CROWDED" />
        <el-option label="超载" value="OVERLOADED" />
      </el-select>
    </div>

    <div class="spot-grid">
      <el-card
        v-for="(spot, i) in filteredSpots"
        :key="spot.spotId"
        class="spot-card sf-fade-up"
        :style="{ animationDelay: `${Math.min(i * 0.04, 0.4)}s` }"
        shadow="hover"
        @click="openDetail(spot)"
      >
        <div class="spot-header">
          <span class="spot-name">{{ spot.name }}</span>
          <LevelTag :level="spot.level" />
        </div>
        <div class="spot-ring">
          <CapacityRing :current="spot.currentVisitors" :capacity="spot.capacity" />
        </div>
        <div class="spot-trend">
          <FlowLine :data="localTrends.get(spot.spotId) || []" />
        </div>
      </el-card>
    </div>

    <el-drawer v-model="drawerVisible" :title="drawerSpot?.name || '景点详情'" size="420px">
      <div v-if="drawerSpot" class="drawer-content">
        <div class="drawer-info">
          <div class="info-row">
            <span class="info-label">当前在园</span>
            <span class="info-value">{{ drawerSpot.currentVisitors }} / {{ drawerSpot.capacity }}</span>
          </div>
          <div class="info-row">
            <span class="info-label">全日容量</span>
            <span class="info-value">{{ drawerSpot.dailyCapacity }}</span>
          </div>
          <div class="info-row">
            <span class="info-label">累计入园</span>
            <span class="info-value">{{ drawerSpot.totalIn }}</span>
          </div>
          <div class="info-row">
            <span class="info-label">到达率</span>
            <span class="info-value">{{ drawerSpot.arrivalRate }}/h</span>
          </div>
          <div class="info-row">
            <span class="info-label">离开率</span>
            <span class="info-value">{{ drawerSpot.departureRate }}/h</span>
          </div>
        </div>

        <div class="drawer-chart">
          <div class="sub-title">近1小时趋势</div>
          <div ref="bigChartRef" class="big-chart"></div>
        </div>
      </div>
    </el-drawer>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, onMounted, onUnmounted, watch } from 'vue'
import * as echarts from 'echarts'
import LevelTag from '@/components/LevelTag.vue'
import CapacityRing from '@/components/charts/CapacityRing.vue'
import FlowLine from '@/components/charts/FlowLine.vue'
import { usePolling } from '@/composables/usePolling'
import { getSpots, getSpotFlow } from '@/api'
import type { Spot, FlowPoint } from '@/api'

const spots = ref<Spot[]>([])
const search = ref('')
const filterLevel = ref('')
const drawerVisible = ref(false)
const drawerSpot = ref<Spot | null>(null)
const bigChartRef = ref<HTMLDivElement>()
let bigChart: echarts.ECharts | null = null

const localTrends = ref<Map<string, number[]>>(new Map())

const filteredSpots = computed(() => {
  return spots.value.filter((s) => {
    const matchSearch = !search.value || s.name.includes(search.value)
    const matchLevel = !filterLevel.value || s.level === filterLevel.value
    return matchSearch && matchLevel
  })
})

async function fetchSpots() {
  try {
    const res = await getSpots()
    const next = res.data
    for (const s of next) {
      const arr = localTrends.value.get(s.spotId) || []
      arr.push(s.currentVisitors)
      if (arr.length > 20) arr.shift()
      localTrends.value.set(s.spotId, arr)
    }
    spots.value = next
  } catch {
    // silent
  }
}

usePolling(fetchSpots, 3000)

function openDetail(spot: Spot) {
  drawerSpot.value = spot
  drawerVisible.value = true
}

async function initBigChart() {
  if (!bigChartRef.value) return
  bigChart = echarts.init(bigChartRef.value)
  window.addEventListener('resize', resizeBigChart)
  await updateBigChart()
}

async function updateBigChart() {
  if (!bigChart || !drawerSpot.value) return
  let data: FlowPoint[] = []
  try {
    const res = await getSpotFlow(drawerSpot.value.spotId)
    data = res.data
  } catch {
    data = []
  }
  const x = data.map((d) => {
    const t = new Date(d.timestamp)
    return `${String(t.getHours()).padStart(2, '0')}:${String(t.getMinutes()).padStart(2, '0')}`
  })
  const y = data.map((d) => d.visitors)
  bigChart.setOption({
    tooltip: { trigger: 'axis' },
    xAxis: { type: 'category', data: x, axisLabel: { rotate: 30, fontSize: 11 } },
    yAxis: { type: 'value' },
    grid: { top: 24, right: 16, bottom: 48, left: 48 },
    series: [
      {
        type: 'line',
        data: y,
        smooth: true,
        areaStyle: {
          color: new (echarts as any).graphic.LinearGradient(0, 0, 0, 1, [
            { offset: 0, color: 'rgba(20,184,166,0.3)' },
            { offset: 1, color: 'rgba(20,184,166,0.04)' },
          ]),
        },
        lineStyle: { color: '#0d9488', width: 2 },
      },
    ],
  })
}

function resizeBigChart() {
  bigChart?.resize()
}

function disposeBigChart() {
  window.removeEventListener('resize', resizeBigChart)
  bigChart?.dispose()
  bigChart = null
}

watch(drawerVisible, (visible) => {
  if (visible) {
    setTimeout(() => {
      initBigChart()
    }, 300)
  } else {
    disposeBigChart()
  }
})

watch(() => drawerSpot.value?.spotId, () => {
  if (drawerVisible.value) {
    updateBigChart()
  }
})

onMounted(() => {
  fetchSpots()
})
onUnmounted(() => {
  disposeBigChart()
})
</script>

<style scoped>
.spots-page {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.toolbar {
  display: flex;
  gap: 12px;
  flex-wrap: wrap;
}

.spot-grid {
  display: grid;
  grid-template-columns: repeat(4, 1fr);
  gap: 16px;
}

.spot-card {
  cursor: pointer;
  transition: transform 0.22s cubic-bezier(0.22, 1, 0.36, 1), box-shadow 0.22s ease;
}

.spot-card:hover {
  transform: translateY(-3px);
}

.spot-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  margin-bottom: 8px;
  padding-bottom: 8px;
  border-bottom: 1px dashed #e6f0ee;
}

.spot-name {
  font-weight: 600;
  color: var(--sf-ink-soft);
  font-size: 15px;
}

.spot-ring {
  height: 120px;
}

.spot-trend {
  margin-top: 8px;
}

.drawer-content {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.drawer-info {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 12px;
}

.info-row {
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.info-label {
  font-size: 12px;
  color: #909399;
}

.info-value {
  font-size: 14px;
  font-weight: 600;
  color: #303133;
}

.sub-title {
  font-weight: 600;
  color: #303133;
  margin-bottom: 8px;
}

.big-chart {
  width: 100%;
  height: 260px;
}

@media (max-width: 1199px) {
  .spot-grid {
    grid-template-columns: repeat(2, 1fr);
  }
}

@media (max-width: 767px) {
  .spot-grid {
    grid-template-columns: 1fr;
  }
}
</style>
