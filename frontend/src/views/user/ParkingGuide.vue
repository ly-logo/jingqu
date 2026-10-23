<template>
  <div class="page">
    <el-card v-for="p in lots" :key="p.parkingId" shadow="never" class="card">
      <div class="lot-row">
        <div class="lot-info">
          <div class="lot-name">{{ p.name }}</div>
          <div class="lot-distance">距主入口步行约 {{ p.distanceM }} 米</div>
        </div>
        <NavButton :name="p.name" :latitude="p.latitude" :longitude="p.longitude" />
      </div>
      <div class="lot-status">
        <span class="free" :class="{ tight: p.freeSpaces < 20 }">
          剩余 {{ p.freeSpaces }} 个车位
        </span>
        <el-tag :type="p.freeSpaces < 20 ? 'danger' : 'success'" size="small">
          {{ p.statusText }}
        </el-tag>
      </div>
    </el-card>
    <el-empty v-if="!lots.length" description="暂无停车场信息" />
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import NavButton from '@/components/user/NavButton.vue'
import { getUserParking } from '@/api'
import type { UserParking } from '@/api'
import { usePolling } from '@/composables/usePolling'

const lots = ref<UserParking[]>([])

async function fetchData() {
  const res = await getUserParking()
  lots.value = res.data
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

.lot-row {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 10px;
}

.lot-name {
  font-size: 16px;
  font-weight: 600;
  color: #303133;
}

.lot-distance {
  font-size: 12px;
  color: #909399;
  margin-top: 2px;
}

.lot-status {
  display: flex;
  align-items: center;
  justify-content: space-between;
  margin-top: 10px;
  padding-top: 10px;
  border-top: 1px solid #f5f7fa;
}

.free {
  font-size: 15px;
  font-weight: 600;
  color: #67c23a;
}

.free.tight {
  color: #f56c6c;
}
</style>
