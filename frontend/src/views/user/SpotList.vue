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
      v-for="s in filtered"
      :key="s.spotId"
      shadow="never"
      class="card spot-card"
      @click="$router.push(`/spots/${s.spotId}`)"
    >
      <div class="spot-row">
        <div class="spot-info">
          <div class="spot-name">{{ s.name }}</div>
          <div class="spot-meta">
            <QueueBadge :minutes="s.queueMinutes" />
            <span class="open-status" :class="{ closed: !isOpen(s) }">
              {{ isOpen(s) ? '开放中' : '已关闭' }}
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

.card {
  border-radius: 6px;
}

.spot-card {
  cursor: pointer;
}

.spot-card :deep(.el-card__body) {
  padding: 14px 16px;
}

.spot-row {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 10px;
}

.spot-name {
  font-size: 16px;
  font-weight: 600;
  color: #303133;
}

.spot-meta {
  display: flex;
  align-items: center;
  gap: 8px;
  margin-top: 6px;
}

.open-status {
  font-size: 12px;
  color: #67c23a;
}

.open-status.closed {
  color: #909399;
}
</style>
