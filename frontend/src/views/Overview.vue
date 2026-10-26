<template>
  <div class="overview">
    <!-- 欢迎横幅 -->
    <div class="hero sf-fade-up">
      <div class="hero-text">
        <div class="hero-title">{{ greeting }}，{{ authStore.username }}</div>
        <div class="hero-sub">{{ todayText }} · 武陵源景区运行态势一屏总览</div>
      </div>
      <div class="hero-ornament" aria-hidden="true">🏞</div>
    </div>

    <!-- 统计卡条 -->
    <div class="stat-row">
      <div class="stat-col">
        <StatCard title="全景区在园人数" :value="systemStatus?.currentVisitors ?? 0" suffix="/ 20000">
          <template #icon>
            <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.7" stroke-linecap="round" stroke-linejoin="round">
              <circle cx="9" cy="8" r="3.2" />
              <path d="M3.5 19a5.5 5.5 0 0 1 11 0" />
              <path d="M16 5.6a3.2 3.2 0 0 1 0 6" />
              <path d="M17.5 13.5A5.5 5.5 0 0 1 20.5 19" />
            </svg>
          </template>
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
        <StatCard title="拥挤景点数" :value="systemStatus?.crowdedSpotCount ?? 0">
          <template #icon>
            <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.7" stroke-linecap="round" stroke-linejoin="round">
              <path d="M12 3.6L21.4 20H2.6L12 3.6z" />
              <path d="M12 10v4" />
              <circle cx="12" cy="16.8" r="0.9" fill="currentColor" stroke="none" />
            </svg>
          </template>
        </StatCard>
      </div>
      <div class="stat-col">
        <StatCard title="剩余车位总数" :value="systemStatus?.freeSpaces ?? 0">
          <template #icon>
            <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.7" stroke-linecap="round" stroke-linejoin="round">
              <rect x="4" y="4" width="16" height="16" rx="4" />
              <path d="M10 16V8h3a2.5 2.5 0 0 1 0 5h-3" />
            </svg>
          </template>
        </StatCard>
      </div>
      <div class="stat-col">
        <StatCard
          title="摆渡车在线"
          :value="`${systemStatus?.shuttleOnline ?? 0}/${systemStatus?.shuttleTotal ?? 0}`"
        >
          <template #icon>
            <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.7" stroke-linecap="round" stroke-linejoin="round">
              <rect x="3" y="5" width="18" height="12" rx="2.5" />
              <path d="M3 10.5h18" />
              <circle cx="7.5" cy="19" r="1.6" />
              <circle cx="16.5" cy="19" r="1.6" />
            </svg>
          </template>
        </StatCard>
      </div>
    </div>

    <!-- 热力图 + 等级分布 -->
    <div class="chart-row">
      <div class="chart-col">
        <el-card shadow="never" class="chart-card sf-fade-up" style="animation-delay: 0.08s">
          <template #header>
            <div class="card-header sf-section-title">景区热力图</div>
          </template>
          <HeatMap :spots="spots" />
        </el-card>
      </div>
      <div class="chart-col">
        <el-card shadow="never" class="chart-card sf-fade-up" style="animation-delay: 0.16s">
          <template #header>
            <div class="card-header sf-section-title">拥挤等级分布</div>
          </template>
          <LevelPie :spots="spots" />
        </el-card>
      </div>
    </div>

    <!-- 资源总览 -->
    <el-card shadow="never" class="resource-card sf-fade-up" style="animation-delay: 0.24s">
      <template #header>
        <div class="card-header sf-section-title">资源总览</div>
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
import { useAuthStore } from '@/stores/auth'
import { getSystemStatus, getSpots, getParkings, getShuttles } from '@/api'
import type { SystemStatus, Spot, ParkingLot, Shuttle } from '@/api'

const authStore = useAuthStore()

const greeting = computed(() => {
  const h = new Date().getHours()
  if (h < 6) return '凌晨好'
  if (h < 9) return '早上好'
  if (h < 12) return '上午好'
  if (h < 14) return '中午好'
  if (h < 18) return '下午好'
  return '晚上好'
})

const todayText = new Date().toLocaleDateString('zh-CN', {
  month: 'long',
  day: 'numeric',
  weekday: 'long',
})

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

/* ---------- 欢迎横幅 ---------- */
.hero {
  position: relative;
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 22px 28px;
  border-radius: var(--sf-radius);
  overflow: hidden;
  color: #fff;
  background:
    radial-gradient(360px 160px at 88% -30%, rgba(232, 178, 90, 0.28) 0%, transparent 60%),
    radial-gradient(420px 200px at 4% 130%, rgba(45, 212, 191, 0.3) 0%, transparent 55%),
    linear-gradient(125deg, #0f5f59 0%, #0a4a45 55%, #072e2b 100%);
  box-shadow: 0 14px 34px rgba(8, 56, 52, 0.26);
}

.hero-title {
  font-size: 20px;
  font-weight: 700;
  letter-spacing: 1px;
}

.hero-sub {
  margin-top: 6px;
  font-size: 13px;
  color: rgba(255, 255, 255, 0.68);
  letter-spacing: 0.5px;
}

.hero-ornament {
  width: 132px;
  height: 82px;
  flex-shrink: 0;
  animation: sf-float 6s ease-in-out infinite;
}

.stat-row {
  display: flex;
  gap: 16px;
  flex-wrap: wrap;
}

.stat-col {
  flex: 1 1 calc(25% - 12px);
  min-width: 200px;
  animation: sf-fade-up 0.45s cubic-bezier(0.22, 1, 0.36, 1) both;
}

/* 统计卡依次浮现 */
.stat-col:nth-child(1) { animation-delay: 0s; }
.stat-col:nth-child(2) { animation-delay: 0.06s; }
.stat-col:nth-child(3) { animation-delay: 0.12s; }
.stat-col:nth-child(4) { animation-delay: 0.18s; }

.chart-row {
  display: flex;
  gap: 16px;
  flex-wrap: wrap;
}

.chart-col {
  flex: 1 1 calc(50% - 8px);
  min-width: 280px;
}

.card-header {
  font-weight: 600;
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
  display: flex;
  align-items: center;
  gap: 6px;
  font-size: 13px;
  font-weight: 600;
  margin-bottom: 12px;
  color: var(--sf-text-secondary);
  letter-spacing: 0.5px;
}

.sub-header::before {
  content: '';
  width: 3px;
  height: 11px;
  border-radius: 2px;
  background: linear-gradient(180deg, #e8b25a, #d99a33);
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
  color: var(--sf-text);
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
  padding: 6px 10px;
  border-radius: 8px;
  background: #f4faf9;
  border: 1px solid #e6f2f0;
  transition: background 0.2s ease, transform 0.2s ease;
}

.shuttle-item:hover {
  background: #ecf7f5;
  transform: translateX(2px);
}

.shuttle-info {
  font-size: 13px;
  color: var(--sf-text-secondary);
}

@media (max-width: 768px) {
  .stat-col,
  .chart-col,
  .resource-col {
    flex: 1 1 100%;
  }
}
</style>
