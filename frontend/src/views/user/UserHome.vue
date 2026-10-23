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
      class="alert"
    />

    <!-- 当日客流时段预测 -->
    <el-card shadow="never" class="card">
      <div class="card-title">今日客流时段预测</div>
      <ForecastBar
        v-if="forecast"
        :points="forecast.points"
        :peak-start="forecast.peakStart"
        :peak-end="forecast.peakEnd"
      />
    </el-card>

    <!-- 避峰榜单 -->
    <el-card shadow="never" class="card">
      <div class="card-title">避峰榜单 · 人少舒适</div>
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
      <div class="card-title">拥堵预警榜单</div>
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

.card {
  border-radius: 6px;
}

.card-title {
  font-size: 14px;
  font-weight: 600;
  color: #303133;
  margin-bottom: 10px;
}

.conclusion-card {
  text-align: center;
}

.conclusion-title {
  font-size: 13px;
  color: #909399;
}

.conclusion-main {
  font-size: 40px;
  font-weight: 700;
  letter-spacing: 8px;
  margin: 6px 0;
}

.conclusion-sub {
  font-size: 12px;
  color: #909399;
  cursor: pointer;
}

.toggle {
  margin-left: 6px;
  color: #409eff;
}

.conclusion-detail {
  margin-top: 8px;
  font-size: 12px;
  color: #606266;
  background: #f5f7fa;
  border-radius: 4px;
  padding: 8px;
}

.alert {
  border-radius: 6px;
}

.rank-row {
  display: flex;
  align-items: center;
  gap: 10px;
  padding: 8px 0;
  border-bottom: 1px solid #f5f7fa;
  cursor: pointer;
}

.rank-row:last-of-type {
  border-bottom: none;
}

.rank-no {
  width: 20px;
  height: 20px;
  border-radius: 50%;
  color: #fff;
  font-size: 12px;
  display: flex;
  align-items: center;
  justify-content: center;
  flex-shrink: 0;
}

.rank-no.avoid {
  background: #67c23a;
}

.rank-no.crowded {
  background: #f56c6c;
}

.rank-name {
  flex: 1;
  font-size: 14px;
  color: #303133;
}

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
  font-size: 26px;
}

.map-text {
  flex: 1;
}

.map-title {
  font-size: 14px;
  font-weight: 600;
  color: #303133;
}

.map-sub {
  font-size: 12px;
  color: #909399;
  margin-top: 2px;
}

.map-arrow {
  font-size: 20px;
  color: #c0c4cc;
}
</style>
