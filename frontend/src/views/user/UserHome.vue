<template>
  <div class="page">
    <!-- 实时拥挤结论 -->
    <el-card shadow="never" class="card conclusion-card">
      <div class="conclusion-title">当前全景区</div>
      <div class="conclusion-main" :style="{ color: levelColor }">
        {{ data?.conclusion.text || '--' }}
      </div>
      <div class="conclusion-sub" @click="showDetail = !showDetail">
        更新于 {{ shortTime(data?.updatedAt) }}
        <span class="toggle">{{ showDetail ? '收起 ▲' : '详情 ▼' }}</span>
      </div>
      <div v-if="showDetail && data" class="conclusion-detail">
        在园约 {{ data.conclusion.currentVisitors }} 人（景区承载 {{ data.conclusion.totalCapacity }} 人），当前
        {{ data.conclusion.crowdedSpotCount }} 个景点拥挤
      </div>
    </el-card>

    <!-- 客流错峰预警 -->
    <el-alert
      v-if="data?.peakAdvice"
      :title="data.peakAdvice.message"
      :type="data.peakAdvice.isPeak ? 'warning' : 'success'"
      :closable="false"
      show-icon
      class="alert sf-fade-up"
    />

    <!-- 当日客流时段预测 -->
    <el-card shadow="never" class="card">
      <div class="card-title sf-section-title">今日客流时段预测</div>
      <ForecastBar
        v-if="forecast"
        :points="forecast.points"
        :peak-start="forecast.peakStart"
        :peak-end="forecast.peakEnd"
      />
    </el-card>

    <!-- 避峰榜单 -->
    <el-card shadow="never" class="card">
      <div class="card-title sf-section-title">避峰榜单 · 人少舒适</div>
      <div
        v-for="(s, i) in avoidList"
        :key="s.spotId"
        class="rank-row"
        @click="$router.push(`/spots/${s.spotId}`)"
      >
        <span class="rank-no avoid">{{ i + 1 }}</span>
        <span class="rank-name">{{ s.name }}</span>
        <LevelTag :level="s.level" />
        <QueueBadge :minutes="s.queueMinutes" />
      </div>
      <el-empty v-if="!avoidList.length" description="暂无数据" :image-size="48" />
    </el-card>

    <!-- 拥堵预警榜单 -->
    <el-card shadow="never" class="card">
      <div class="card-title sf-section-title">拥堵预警榜单</div>
      <div
        v-for="(s, i) in crowdedList"
        :key="s.spotId"
        class="rank-row"
        @click="$router.push(`/spots/${s.spotId}`)"
      >
        <span class="rank-no crowded">{{ i + 1 }}</span>
        <span class="rank-name">{{ s.name }}</span>
        <LevelTag :level="s.level" />
        <QueueBadge :minutes="s.queueMinutes" />
      </div>
      <el-empty v-if="!crowdedList.length" description="当前无拥挤景点" :image-size="48" />
    </el-card>

    <!-- 景区底图入口 -->
    <el-card shadow="never" class="card map-entry" @click="$router.push('/map')">
      <div class="map-entry-inner">
        <span class="map-icon">🗺</span>
        <div class="map-text">
          <div class="map-title">景区底图</div>
          <div class="map-sub">找卫生间 · 停车场 · 车站</div>
        </div>
        <span class="map-arrow">›</span>
      </div>
    </el-card>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue'
import LevelTag from '@/components/LevelTag.vue'
import QueueBadge from '@/components/user/QueueBadge.vue'
import ForecastBar from '@/components/charts/ForecastBar.vue'
import { getUserOverview, getUserRankings, getUserForecast } from '@/api'
import type { OverviewData, RankingsData, ForecastData } from '@/api'
import { usePolling } from '@/composables/usePolling'
import { LEVEL_COLORS } from '@/utils/level'

const data = ref<OverviewData | null>(null)
const avoid = ref<RankingsData | null>(null)
const crowded = ref<RankingsData | null>(null)
const forecast = ref<ForecastData | null>(null)
const showDetail = ref(false)

const avoidList = computed(() => avoid.value?.list || [])
const crowdedList = computed(() => crowded.value?.list || [])
const levelColor = computed(() =>
  data.value ? LEVEL_COLORS[data.value.conclusion.level] : '#909399',
)

function shortTime(s?: string): string {
  if (!s) return '--'
  const m = s.match(/(\d{2}:\d{2}):\d{2}/)
  return m ? m[1] : s
}

async function fetchData() {
  const [ov, av, cd, fc] = await Promise.allSettled([
    getUserOverview(),
    getUserRankings('avoid'),
    getUserRankings('crowded'),
    getUserForecast(),
  ])
  if (ov.status === 'fulfilled') data.value = ov.value.data
  if (av.status === 'fulfilled') avoid.value = av.value.data
  if (cd.status === 'fulfilled') crowded.value = cd.value.data
  if (fc.status === 'fulfilled') forecast.value = fc.value.data
}

usePolling(fetchData, 30000)
</script>

<style scoped>
.page {
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.card-title {
  font-size: 14px;
  margin-bottom: 12px;
}

/* ---------- 实时结论卡：青碧瓷感 hero ---------- */
.conclusion-card {
  text-align: center;
  border: none;
  background:
    radial-gradient(280px 140px at 88% -20%, rgba(232, 178, 90, 0.16) 0%, transparent 65%),
    radial-gradient(320px 180px at 6% 118%, rgba(45, 212, 191, 0.2) 0%, transparent 60%),
    linear-gradient(150deg, #0f5f59 0%, #0a4a45 55%, #072e2b 100%);
  box-shadow: 0 12px 30px rgba(8, 56, 52, 0.28);
}

.conclusion-card :deep(.el-card__body) {
  padding: 22px 18px 18px;
}

.conclusion-title {
  font-size: 12px;
  color: rgba(255, 255, 255, 0.62);
  letter-spacing: 4px;
}

.conclusion-main {
  font-size: 40px;
  font-weight: 700;
  letter-spacing: 8px;
  margin: 8px 0 6px;
  text-shadow: 0 2px 14px rgba(0, 0, 0, 0.28);
}

.conclusion-sub {
  font-size: 12px;
  color: rgba(255, 255, 255, 0.6);
  cursor: pointer;
}

.toggle {
  margin-left: 6px;
  color: #7be3d7;
}

.conclusion-detail {
  margin-top: 10px;
  font-size: 12px;
  color: rgba(255, 255, 255, 0.85);
  background: rgba(255, 255, 255, 0.1);
  border: 1px solid rgba(255, 255, 255, 0.12);
  border-radius: 8px;
  padding: 8px 10px;
  backdrop-filter: blur(4px);
}

.alert {
  border-radius: 10px;
}

/* ---------- 榜单行 ---------- */
.rank-row {
  display: flex;
  align-items: center;
  gap: 10px;
  padding: 9px 8px;
  border-bottom: 1px solid #eef4f3;
  border-radius: 8px;
  cursor: pointer;
  transition:
    background 0.2s ease,
    transform 0.2s ease;
}

.rank-row:hover {
  background: #f2faf9;
  transform: translateX(3px);
}

.rank-row:last-of-type {
  border-bottom: none;
}

.rank-no {
  width: 21px;
  height: 21px;
  border-radius: 7px;
  color: #fff;
  font-size: 12px;
  font-weight: 600;
  display: flex;
  align-items: center;
  justify-content: center;
  flex-shrink: 0;
}

.rank-no.avoid {
  background: linear-gradient(135deg, #34d399, #10a06e);
  box-shadow: 0 3px 8px rgba(16, 160, 110, 0.32);
}

.rank-no.crowded {
  background: linear-gradient(135deg, #fb7a6b, #f04a3a);
  box-shadow: 0 3px 8px rgba(240, 74, 58, 0.3);
}

.rank-name {
  flex: 1;
  font-size: 14px;
  color: var(--sf-text);
}

/* ---------- 底图入口 ---------- */
.map-entry {
  cursor: pointer;
}

.map-entry :deep(.el-card__body) {
  padding: 14px 16px;
}

.map-entry-inner {
  display: flex;
  align-items: center;
  gap: 12px;
}

.map-icon {
  width: 42px;
  height: 42px;
  flex-shrink: 0;
  display: flex;
  align-items: center;
  justify-content: center;
  color: #0f766e;
  border-radius: 12px;
  background: linear-gradient(135deg, #e9f7f5, #d3efeb);
  border: 1px solid #cbe8e4;
}

.map-text {
  flex: 1;
}

.map-title {
  font-size: 14px;
  font-weight: 600;
  color: var(--sf-ink-soft);
}

.map-sub {
  font-size: 12px;
  color: var(--sf-text-secondary);
  margin-top: 2px;
}

.map-arrow {
  font-size: 20px;
  color: #b6ccc8;
  transition: transform 0.22s ease, color 0.22s ease;
}

.map-entry:hover .map-arrow {
  transform: translateX(3px);
  color: var(--sf-primary);
}
</style>
