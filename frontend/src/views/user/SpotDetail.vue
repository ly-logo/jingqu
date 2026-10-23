<template>
  <div class="page">
    <div v-if="detail" class="detail">
      <!-- 头部 -->
      <el-card shadow="never" class="card">
        <div class="head-row">
          <el-button text @click="$router.back()">
            <el-icon><ArrowLeft /></el-icon>
          </el-button>
          <span class="spot-name">{{ detail.name }}</span>
          <LevelTag :level="detail.level" />
        </div>
        <div class="head-meta">
          <QueueBadge :minutes="detail.queueMinutes" />
          <span class="open-time">开放时间 {{ detail.openTime }} ~ {{ detail.closeTime }}</span>
        </div>
      </el-card>

      <!-- 客流趋势（弱化展示） -->
      <el-card shadow="never" class="card">
        <div class="card-title">今日客流趋势</div>
        <FlowLine :data="trendValues" :x-data="trendHours" />
      </el-card>

      <!-- 周边配套导航 -->
      <el-card shadow="never" class="card">
        <div class="card-title">周边配套</div>
        <div v-for="p in detail.pois" :key="p.poiId" class="poi-row">
          <span class="poi-icon">{{ iconOf(p.category) }}</span>
          <div class="poi-info">
            <div class="poi-name">{{ p.name }}</div>
            <div class="poi-distance">距离约 {{ p.distanceM }} 米</div>
          </div>
          <NavButton :name="p.name" :latitude="p.latitude" :longitude="p.longitude" />
        </div>
        <el-empty v-if="!detail.pois.length" description="暂无周边点位" :image-size="48" />
      </el-card>
    </div>
    <el-skeleton v-else :rows="5" animated class="card" />
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue'
import { useRoute } from 'vue-router'
import { ArrowLeft } from '@element-plus/icons-vue'
import LevelTag from '@/components/LevelTag.vue'
import QueueBadge from '@/components/user/QueueBadge.vue'
import NavButton from '@/components/user/NavButton.vue'
import FlowLine from '@/components/charts/FlowLine.vue'
import { getSpotDetail } from '@/api'
import type { SpotDetailData, PoiCategory } from '@/api'
import { usePolling } from '@/composables/usePolling'
import { POI_CATEGORY_ICONS } from '@/utils/level'

const route = useRoute()
const detail = ref<SpotDetailData | null>(null)

const trendValues = computed(() => detail.value?.trend.map((p) => p.visitors) || [])
const trendHours = computed(() => detail.value?.trend.map((p) => `${p.hour}时`) || [])

function iconOf(category: PoiCategory): string {
  return POI_CATEGORY_ICONS[category] || '📍'
}

async function fetchData() {
  const res = await getSpotDetail(String(route.params.id))
  detail.value = res.data
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

.head-row {
  display: flex;
  align-items: center;
  gap: 10px;
}

.spot-name {
  flex: 1;
  font-size: 18px;
  font-weight: 600;
  color: #303133;
}

.head-meta {
  display: flex;
  align-items: center;
  gap: 12px;
  margin-top: 10px;
}

.open-time {
  font-size: 12px;
  color: #909399;
}

.poi-row {
  display: flex;
  align-items: center;
  gap: 10px;
  padding: 9px 0;
  border-bottom: 1px solid #f5f7fa;
}

.poi-row:last-of-type {
  border-bottom: none;
}

.poi-icon {
  font-size: 18px;
  flex-shrink: 0;
}

.poi-info {
  flex: 1;
  min-width: 0;
}

.poi-name {
  font-size: 14px;
  color: #303133;
}

.poi-distance {
  font-size: 12px;
  color: #909399;
  margin-top: 2px;
}
</style>
