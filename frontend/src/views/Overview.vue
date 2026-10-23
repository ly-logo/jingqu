<template>
  <div class="overview">
    <!-- 统计卡条 -->
    <div class="stat-row">
      <div class="stat-col">
        <StatCard title="全景区在园人数" :value="systemStatus?.currentVisitors ?? 0" suffix="/ 20000">
          <el-progress
            :percentage="visitorRatio"
            :color="progressColor"
            :show-text="false"
            :stroke-width="6"
            style="margin-top: 8px"
          />
        </StatCard>
      </div>
      <div class="stat-col">
        <StatCard title="拥挤景点数" :value="systemStatus?.crowdedSpotCount ?? 0" />
      </div>
      <div class="stat-col">
        <StatCard title="剩余车位总数" :value="systemStatus?.freeSpaces ?? 0" />
      </div>
      <div class="stat-col">
        <StatCard
          title="摆渡车在线"
          :value="`${systemStatus?.shuttleOnline ?? 0}/${systemStatus?.shuttleTotal ?? 0}`"
        />
      </div>
    </div>

    <!-- 热力图 + 等级分布 -->
    <div class="chart-row">
      <div class="chart-col">
        <el-card shadow="never" class="chart-card">
          <template #header>
            <div class="card-header">景区热力图</div>
          </template>
          <HeatMap :spots="spots" />
        </el-card>
      </div>
      <div class="chart-col">
        <el-card shadow="never" class="chart-card">
          <template #header>
            <div class="card-header">拥挤等级分布</div>
          </template>
          <LevelPie :spots="spots" />
        </el-card>
      </div>
    </div>

    <!-- 资源总览 -->
    <el-card shadow="never" class="resource-card">
      <template #header>
        <div class="card-header">资源总览</div>
      </template>
      <div class="resource-row">
        <div class="resource-col">
          <div class="sub-header">停车场剩余车位</div>
          <div v-for="p in parkings" :key="p.parkingId" class="parking-bar">
            <div class="bar-label">{{ p.name }}</div>
            <el-progress
              :percentage="Math.round((p.freeSpaces / p.totalSpaces) * 100)"
              :color="p.freeSpaces < 30 ? '#F56C6C' : '#67C23A'"
              :text-inside="true"
              :stroke-width="18"
              :format="() => `${p.freeSpaces} / ${p.totalSpaces}`"
              class="bar-progress"
            />
          </div>
        </div>
        <div class="resource-col">
          <div class="sub-header">摆渡车运行状态</div>
          <div class="shuttle-list">
            <div
              v-for="s in shuttles.slice(0, 8)"
              :key="s.shuttleId"
              class="shuttle-item"
            >
              <el-tag
                :type="s.status === 'RUNNING' ? 'success' : s.status === 'IDLE' ? 'info' : 'warning'"
                size="small"
              >
                {{ s.name }}
              </el-tag>
              <span class="shuttle-info">{{ s.currentStation }} · 载客 {{ s.passengerCount }}/{{ s.capacity }}</span>
            </div>
          </div>
        </div>
      </div>
    </el-card>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue'
import StatCard from '@/components/StatCard.vue'
import HeatMap from '@/components/charts/HeatMap.vue'
import LevelPie from '@/components/charts/LevelPie.vue'
import { usePolling } from '@/composables/usePolling'
import { getSystemStatus, getSpots, getParkings, getShuttles } from '@/api'
import type { SystemStatus, Spot, ParkingLot, Shuttle } from '@/api'

const systemStatus = ref<SystemStatus | null>(null)
const spots = ref<Spot[]>([])
const parkings = ref<ParkingLot[]>([])
const shuttles = ref<Shuttle[]>([])

const visitorRatio = computed(() => {
  if (!systemStatus.value) return 0
  return Math.min(100, Math.round((systemStatus.value.currentVisitors / systemStatus.value.totalCapacity) * 100))
})

const progressColor = computed(() => {
  const r = visitorRatio.value / 100
  if (r < 0.6) return '#67C23A'
  if (r < 0.9) return '#E6A23C'
  if (r < 1.0) return '#FF7A45'
  return '#F56C6C'
})

async function fetchAll() {
  try {
    const [sysRes, spotsRes, parkRes, shuttleRes] = await Promise.all([
      getSystemStatus(),
      getSpots(),
      getParkings(),
      getShuttles(),
    ])
    systemStatus.value = sysRes.data
    spots.value = spotsRes.data
    parkings.value = parkRes.data
    shuttles.value = shuttleRes.data
  } catch {
    // silent in polling
  }
}

usePolling(fetchAll, 3000)
</script>

<style scoped>
.overview {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.stat-row {
  display: flex;
  gap: 16px;
  flex-wrap: wrap;
}

.stat-col {
  flex: 1 1 calc(25% - 12px);
  min-width: 200px;
}

.chart-row {
  display: flex;
  gap: 16px;
  flex-wrap: wrap;
}

.chart-col {
  flex: 1 1 calc(50% - 8px);
  min-width: 280px;
}

.chart-card {
  border-radius: 6px;
}

.card-header {
  font-weight: 600;
  color: #303133;
}

.resource-card {
  border-radius: 6px;
}

.resource-row {
  display: flex;
  gap: 24px;
  flex-wrap: wrap;
}

.resource-col {
  flex: 1 1 calc(50% - 12px);
  min-width: 280px;
}

.sub-header {
  font-weight: 600;
  margin-bottom: 12px;
  color: #606266;
}

.parking-bar {
  display: flex;
  align-items: center;
  gap: 12px;
  margin-bottom: 10px;
}

.bar-label {
  width: 100px;
  font-size: 13px;
  color: #606266;
  white-space: nowrap;
}

.bar-progress {
  flex: 1;
}

.shuttle-list {
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.shuttle-item {
  display: flex;
  align-items: center;
  gap: 8px;
}

.shuttle-info {
  font-size: 13px;
  color: #606266;
}

@media (max-width: 768px) {
  .stat-col,
  .chart-col,
  .resource-col {
    flex: 1 1 100%;
  }
}
</style>
