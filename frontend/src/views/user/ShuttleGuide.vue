<template>
  <div class="page">
    <el-card v-for="r in routes" :key="r.routeName" shadow="never" class="card">
      <div class="route-head">
        <span class="route-name">🚌 {{ r.routeName }}</span>
        <QueueBadge :minutes="r.waitingMinutes" prefix="候车" :tone="'normal'" />
      </div>
      <div class="stations">
        <template v-for="(st, i) in r.stations" :key="i">
          <span v-if="i > 0" class="arrow">→</span>
          <span class="station">{{ st }}</span>
        </template>
      </div>
      <div class="route-meta">全程约 {{ r.totalDistanceM }} 米</div>
    </el-card>
    <el-empty v-if="!routes.length" description="暂无摆渡线路信息" />
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import QueueBadge from '@/components/user/QueueBadge.vue'
import { getUserShuttleRoutes } from '@/api'
import type { ShuttleRouteGroup } from '@/api'
import { usePolling } from '@/composables/usePolling'

const routes = ref<ShuttleRouteGroup[]>([])

async function fetchData() {
  const res = await getUserShuttleRoutes()
  routes.value = res.data
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
  padding: 14px 16px;
}

.route-head {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 10px;
}

.route-name {
  font-size: 16px;
  font-weight: 600;
  color: #303133;
}

.stations {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  gap: 6px;
  margin-top: 12px;
  padding: 10px;
  background: #f5f7fa;
  border-radius: 6px;
}

.station {
  font-size: 13px;
  color: #303133;
  background: #fff;
  border: 1px solid #e4e7ed;
  border-radius: 4px;
  padding: 3px 8px;
}

.arrow {
  color: #c0c4cc;
  font-size: 12px;
}

.route-meta {
  margin-top: 8px;
  font-size: 12px;
  color: #909399;
}
</style>
