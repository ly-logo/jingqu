<template>
  <div class="page">
    <!-- 类型过滤 -->
    <div class="filters">
      <el-button
        v-for="t in typeOptions"
        :key="t.value"
        size="small"
        :type="filterType === t.value ? 'primary' : 'default'"
        round
        @click="filterType = t.value"
      >
        {{ t.icon }} {{ t.label }}
      </el-button>
    </div>

    <el-card shadow="never" class="card">
      <AMapView :points="filteredMarkers" height="440px" />
      <div class="legend">
        <span><i class="dot gate"></i>闸机</span>
        <span><i class="dot parking"></i>停车场</span>
        <span><i class="dot spot"></i>景点</span>
        <span class="hint">点击标记查看详情</span>
      </div>
    </el-card>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue'
import AMapView from '@/components/user/AMapView.vue'
import { getUserOverview, getUserParking, getUserPois } from '@/api'
import type { MapMarker, UserSpotBrief, UserParking, Poi } from '@/api'
import { usePolling } from '@/composables/usePolling'

const spots = ref<UserSpotBrief[]>([])
const parkings = ref<UserParking[]>([])
const gates = ref<Poi[]>([])
const filterType = ref<MapMarker['type'] | ''>('')

const typeOptions: { value: MapMarker['type'] | ''; label: string; icon: string }[] = [
  { value: '', label: '全部', icon: '🗺' },
  { value: 'gate', label: '闸机', icon: '🛂' },
  { value: 'parking', label: '停车场', icon: '🅿' },
  { value: 'spot', label: '景点', icon: '⛰' },
]

// 景点 / 停车场 / 闸机合并为地图标记点
const markers = computed<MapMarker[]>(() => {
  const list: MapMarker[] = []
  for (const s of spots.value) {
    if (!s.longitude || !s.latitude) continue
    list.push({
      id: s.spotId,
      name: s.name,
      type: 'spot',
      longitude: s.longitude,
      latitude: s.latitude,
      visitors: s.currentVisitors,
      level: s.level,
      levelText: s.levelText,
    })
  }
  for (const p of parkings.value) {
    if (!p.longitude || !p.latitude) continue
    list.push({
      id: p.parkingId,
      name: p.name,
      type: 'parking',
      longitude: p.longitude,
      latitude: p.latitude,
      freeSpaces: p.freeSpaces,
    })
  }
  for (const g of gates.value) {
    if (!g.longitude || !g.latitude) continue
    list.push({
      id: g.poiId,
      name: g.name,
      type: 'gate',
      longitude: g.longitude,
      latitude: g.latitude,
    })
  }
  return list
})

const filteredMarkers = computed(() =>
  filterType.value ? markers.value.filter((m) => m.type === filterType.value) : markers.value,
)

async function fetchData() {
  const [ov, pk, gt] = await Promise.allSettled([
    getUserOverview(),
    getUserParking(),
    getUserPois('GATE'),
  ])
  if (ov.status === 'fulfilled') spots.value = ov.value.data.spots
  if (pk.status === 'fulfilled') parkings.value = pk.value.data
  if (gt.status === 'fulfilled') gates.value = gt.value.data
}

usePolling(fetchData, 30000)
</script>

<style scoped>
.page {
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.filters {
  display: flex;
  gap: 8px;
  flex-wrap: wrap;
}

.card {
  border-radius: 6px;
}

.card :deep(.el-card__body) {
  padding: 8px;
}

.legend {
  display: flex;
  align-items: center;
  gap: 14px;
  padding: 10px 4px 2px;
  font-size: 12px;
  color: #606266;
}

.legend .dot {
  display: inline-block;
  width: 9px;
  height: 9px;
  border-radius: 50%;
  margin-right: 4px;
}

.legend .dot.gate {
  background: #f56c6c;
}

.legend .dot.parking {
  background: #409eff;
}

.legend .dot.spot {
  background: #67c23a;
}

.legend .hint {
  margin-left: auto;
  color: #909399;
}
</style>
