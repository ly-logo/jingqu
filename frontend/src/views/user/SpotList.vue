<template>
  <div class="page">
    <div class="filters">
      <el-input
        v-model="keyword"
        placeholder="搜索景点名称"
        clearable
        prefix-icon="Search"
        size="default"
        class="search"
      />
      <el-select v-model="levelFilter" size="default" class="level-select">
        <el-option label="全部等级" value="" />
        <el-option label="空闲" value="EMPTY" />
        <el-option label="正常" value="NORMAL" />
        <el-option label="适中" value="MODERATE" />
        <el-option label="拥挤" value="CROWDED" />
        <el-option label="超载" value="OVERLOADED" />
      </el-select>
    </div>

    <el-card
      v-for="(s, i) in filtered"
      :key="s.spotId"
      shadow="never"
      class="card spot-card sf-fade-up"
      :style="{ animationDelay: `${Math.min(i * 0.05, 0.4)}s` }"
      @click="$router.push(`/spots/${s.spotId}`)"
    >
      <div class="spot-row">
        <span class="level-bar" :style="{ background: levelColor(s.level) }"></span>
        <div class="spot-info">
          <div class="spot-name">{{ s.name }}</div>
          <div class="spot-meta">
            <QueueBadge :minutes="s.queueMinutes" />
            <span class="open-status" :class="{ closed: !isOpen(s) }">
              {{ isOpen(s) ? '· 开放中' : '· 已关闭' }}
            </span>
          </div>
        </div>
        <LevelTag :level="s.level" size="large" />
      </div>
    </el-card>

    <el-empty v-if="!filtered.length" description="没有符合条件的景点" />
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue'
import { Search } from '@element-plus/icons-vue'
import LevelTag from '@/components/LevelTag.vue'
import QueueBadge from '@/components/user/QueueBadge.vue'
import { getUserOverview } from '@/api'
import type { UserSpotBrief } from '@/api'
import { usePolling } from '@/composables/usePolling'
import { getLevelColor } from '@/utils/level'
import type { CongestionLevel } from '@/api'

function levelColor(level: CongestionLevel): string {
  return getLevelColor(level)
}

const spots = ref<UserSpotBrief[]>([])
const keyword = ref('')
const levelFilter = ref('')

const filtered = computed(() =>
  spots.value.filter(
    (s) =>
      (!keyword.value || s.name.includes(keyword.value)) &&
      (!levelFilter.value || s.level === levelFilter.value),
  ),
)

function isOpen(s: UserSpotBrief): boolean {
  const now = new Date()
  const cur = now.getHours() * 60 + now.getMinutes()
  const [oh, om] = s.openTime.split(':').map(Number)
  const [ch, cm] = s.closeTime.split(':').map(Number)
  return cur >= oh * 60 + om && cur <= ch * 60 + cm
}

async function fetchData() {
  const res = await getUserOverview()
  spots.value = res.data.spots
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
}

.search {
  flex: 1;
}

.level-select {
  width: 110px;
}

/* ---------- 景点卡：等级色条 + 悬浮 ---------- */
.spot-card {
  cursor: pointer;
  transition:
    transform 0.22s cubic-bezier(0.22, 1, 0.36, 1),
    box-shadow 0.22s ease;
}

.spot-card:hover {
  transform: translateY(-2px);
  box-shadow: var(--sf-shadow);
}

.spot-card :deep(.el-card__body) {
  padding: 14px 16px;
}

.spot-row {
  display: flex;
  align-items: center;
  gap: 12px;
}

.level-bar {
  width: 5px;
  height: 38px;
  border-radius: 3px;
  flex-shrink: 0;
  opacity: 0.85;
}

.spot-info {
  flex: 1;
  min-width: 0;
}

.spot-name {
  font-size: 16px;
  font-weight: 600;
  color: var(--sf-ink-soft);
}

.spot-meta {
  display: flex;
  align-items: center;
  gap: 8px;
  margin-top: 6px;
}

.open-status {
  font-size: 12px;
  color: #4caf7d;
}

.open-status.closed {
  color: #9aa8a4;
}
</style>
