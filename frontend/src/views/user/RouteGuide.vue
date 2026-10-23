<template>
  <div class="page">
    <!-- 路线类型切换：切换后地图自动清旧线、画新线并缩放适配 -->
    <el-radio-group v-model="routeType" size="small" class="switch">
      <el-radio-button label="avoid">🌿 避峰路线</el-radio-button>
      <el-radio-button label="classic">🏯 经典路线</el-radio-button>
    </el-radio-group>

    <el-card shadow="never" class="card">
      <AMapView
        :points="markers"
        :route-path="currentRoute?.steps || []"
        :line-color="lineColor"
        :focus-id="focusId"
        height="340px"
      />
    </el-card>

    <el-card shadow="never" class="card">
      <div class="route-head">
        <span class="route-name">{{ currentRoute?.name || '路线推荐' }}</span>
        <span v-if="currentRoute" class="route-est">约 {{ currentRoute.estimatedMinutes }} 分钟</span>
      </div>
      <p v-if="currentRoute?.message" class="route-message">{{ currentRoute.message }}</p>

      <template v-if="currentRoute?.steps.length">
        <div class="step-label">
          途径 {{ currentRoute.steps.length }} 个景点 · 步行约 {{ currentRoute.totalDistanceM }} 米
        </div>
        <div
          v-for="s in currentRoute.steps"
          :key="s.spotId"
          class="step-row"
          :class="{ active: focusId === s.spotId }"
          @click="focusSpot(s.spotId)"
        >
          <span class="step-no">{{ s.order }}</span>
          <span class="step-name">{{ s.name }}</span>
          <LevelTag :level="s.level" />
        </div>
      </template>
      <el-skeleton v-else :rows="3" animated />
    </el-card>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, nextTick } from 'vue'
import AMapView from '@/components/user/AMapView.vue'
import LevelTag from '@/components/LevelTag.vue'
import { getUserRecommendRoutes } from '@/api'
import type { MapMarker, RecommendData } from '@/api'
import { usePolling } from '@/composables/usePolling'

const routeType = ref<'avoid' | 'classic'>('avoid')
const avoidRoute = ref<RecommendData | null>(null)
const classicRoute = ref<RecommendData | null>(null)
const focusId = ref('')

const currentRoute = computed(() => (routeType.value === 'avoid' ? avoidRoute.value : classicRoute.value))

// 经典路线灰色、避峰路线绿色
const lineColor = computed(() => (routeType.value === 'avoid' ? '#67C23A' : '#909399'))

// 路线上的景点即地图标记点
const markers = computed<MapMarker[]>(() =>
  (currentRoute.value?.steps || [])
    .filter((s) => s.longitude && s.latitude)
    .map((s) => ({
      id: s.spotId,
      name: s.name,
      type: 'spot' as const,
      longitude: s.longitude as number,
      latitude: s.latitude as number,
      visitors: s.currentVisitors,
      level: s.level,
      levelText: s.levelText,
    })),
)

// 点击列表景点 → 地图定位到对应标记点（重复点击同一点也能再次定位）
function focusSpot(id: string) {
  if (focusId.value === id) {
    focusId.value = ''
    nextTick(() => (focusId.value = id))
    return
  }
  focusId.value = id
}

async function fetchData() {
  const [av, cl] = await Promise.allSettled([
    getUserRecommendRoutes('avoid'),
    getUserRecommendRoutes('classic'),
  ])
  if (av.status === 'fulfilled') avoidRoute.value = av.value.data
  if (cl.status === 'fulfilled') classicRoute.value = cl.value.data
}

usePolling(fetchData, 30000)
</script>

<style scoped>
.page {
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.card {
  border-radius: 6px;
}

.card :deep(.el-card__body) {
  padding: 8px;
}

.route-head {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 4px;
}

.route-name {
  font-size: 16px;
  font-weight: 600;
  color: #303133;
}

.route-est {
  font-size: 12px;
  color: #409eff;
}

.route-message {
  margin: 10px 4px 0;
  font-size: 13px;
  color: #e6a23c;
}

.steps {
  margin-top: 12px;
}

.step-label {
  font-size: 12px;
  color: #909399;
  margin: 8px 4px 6px;
}

.step-row {
  display: flex;
  align-items: center;
  gap: 10px;
  padding: 8px 4px;
  border-bottom: 1px solid #f5f7fa;
  cursor: pointer;
}

.step-row.active {
  background: #ecf5ff;
  border-radius: 4px;
}

.step-row:last-of-type {
  border-bottom: none;
}

.step-no {
  width: 20px;
  height: 20px;
  border-radius: 50%;
  background: #409eff;
  color: #fff;
  font-size: 12px;
  display: flex;
  align-items: center;
  justify-content: center;
  flex-shrink: 0;
}

.step-name {
  flex: 1;
  font-size: 14px;
  color: #303133;
}
</style>
