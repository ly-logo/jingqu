<template>
  <div class="sys-monitor-page">
    <div class="stat-row">
      <div class="stat-col">
        <StatCard title="流水线总数" :value="pipelineCount" />
      </div>
      <div class="stat-col">
        <StatCard title="运行中数量" :value="runningCount" />
      </div>
      <div class="stat-col">
        <StatCard title="平均队列积压" :value="avgQueue" suffix="条" />
      </div>
      <div class="stat-col">
        <StatCard title="数据源就绪数量" :value="readySourceCount" />
      </div>
    </div>

    <el-card shadow="never" class="section-card">
      <template #header>
        <div class="card-header">流水线状态</div>
      </template>
      <el-table :data="monitor?.pipelines || []" stripe>
        <el-table-column prop="name" label="名称" min-width="200" />
        <el-table-column label="状态" width="120">
          <template #default="{ row }">
            <el-tag :type="row.running ? 'success' : 'danger'" size="small">
              {{ row.running ? '运行中' : '已停止' }}
            </el-tag>
          </template>
        </el-table-column>
        <el-table-column label="队列积压" min-width="140">
          <template #default="{ row }">
            <el-tag v-if="row.queueDepth > 20" type="warning" size="small">
              {{ row.queueDepth }}
            </el-tag>
            <span v-else>{{ row.queueDepth }}</span>
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <el-card shadow="never" class="section-card">
      <template #header>
        <div class="card-header">数据源状态</div>
      </template>
      <el-descriptions :column="2" border>
        <el-descriptions-item
          v-for="src in monitor?.sources || []"
          :key="src.name"
          :label="src.name"
        >
          {{ src.status }}
        </el-descriptions-item>
      </el-descriptions>
    </el-card>

    <el-card shadow="never" class="section-card">
      <template #header>
        <div class="card-header">最近日志</div>
      </template>
      <el-table :data="monitor?.logs || []" stripe>
        <el-table-column prop="time" label="时间" width="180" />
        <el-table-column label="级别" width="100">
          <template #default="{ row }">
            <el-tag :type="LOG_LEVEL_TAG[row.level] || 'info'" size="small">
              {{ row.level }}
            </el-tag>
          </template>
        </el-table-column>
        <el-table-column prop="message" label="内容" min-width="280" show-overflow-tooltip />
      </el-table>
    </el-card>

    <p class="footer-note">
      数据源中「闸机接口」「停车场接口」显示为未接入（预留）属预期，落地时实现对应接入。
    </p>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue'
import StatCard from '@/components/StatCard.vue'
import { getMonitorStatus } from '@/api'
import type { SystemMonitor } from '@/api'
import { usePolling } from '@/composables/usePolling'

const monitor = ref<SystemMonitor | null>(null)

const LOG_LEVEL_TAG: Record<string, 'info' | 'warning' | 'danger'> = {
  INFO: 'info',
  WARN: 'warning',
  ERROR: 'danger',
}

const pipelines = computed(() => monitor.value?.pipelines || [])
const pipelineCount = computed(() => pipelines.value.length)
const runningCount = computed(() => pipelines.value.filter((p) => p.running).length)
const avgQueue = computed(() => {
  const list = pipelines.value
  if (list.length === 0) return '0.0'
  const sum = list.reduce((acc, p) => acc + p.queueDepth, 0)
  return (sum / list.length).toFixed(1)
})
const readySourceCount = computed(
  () => (monitor.value?.sources || []).filter((s) => !s.status.includes('未接入')).length,
)

async function fetch() {
  try {
    const res = await getMonitorStatus()
    monitor.value = res.data
  } catch {
    // silent in polling
  }
}

usePolling(fetch, 5000)
</script>

<style scoped>
.sys-monitor-page {
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
  min-width: 180px;
}

.section-card {
  border-radius: 6px;
}

.card-header {
  font-size: 15px;
  font-weight: 600;
  color: #303133;
}

.footer-note {
  margin: 0;
  font-size: 12px;
  color: #909399;
  line-height: 1.6;
}

@media (max-width: 768px) {
  .stat-col {
    flex: 1 1 100%;
  }
}
</style>
