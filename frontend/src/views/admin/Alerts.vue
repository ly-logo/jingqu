<template>
  <div class="alerts-page">
    <!-- 等级统计 -->
    <div class="stat-row">
      <div class="stat-col">
        <StatCard :title="EVENT_LEVEL_LABELS.CRITICAL" :value="levelCounts.CRITICAL" />
      </div>
      <div class="stat-col">
        <StatCard :title="EVENT_LEVEL_LABELS.HIGH" :value="levelCounts.HIGH" />
      </div>
      <div class="stat-col">
        <StatCard :title="EVENT_LEVEL_LABELS.WARNING" :value="levelCounts.WARNING" />
      </div>
      <div class="stat-col">
        <StatCard :title="EVENT_LEVEL_LABELS.INFO" :value="levelCounts.INFO" />
      </div>
    </div>

    <!-- 筛选 -->
    <div class="toolbar">
      <el-select v-model="filterLevel" placeholder="等级筛选" style="width: 160px">
        <el-option label="全部" value="" />
        <el-option
          v-for="level in LEVEL_OPTIONS"
          :key="level"
          :label="EVENT_LEVEL_LABELS[level]"
          :value="level"
        />
      </el-select>
      <el-select v-model="filterType" placeholder="类型筛选" style="width: 160px">
        <el-option label="全部" value="" />
        <el-option
          v-for="type in ALERT_TYPES"
          :key="type"
          :label="EVENT_TYPE_LABELS[type]"
          :value="type"
        />
      </el-select>
    </div>

    <!-- 告警表格 -->
    <el-card shadow="never" class="table-card">
      <template #header>
        <div class="card-header">实时告警列表</div>
      </template>
      <el-table :data="filteredAlerts" :row-class-name="rowClassName" style="width: 100%">
        <el-table-column prop="createdAt" label="时间" width="170" />
        <el-table-column prop="spotName" label="景点" width="120" />
        <el-table-column label="类型" width="110">
          <template #default="{ row }">
            <el-tag size="small">{{ EVENT_TYPE_LABELS[row.type] }}</el-tag>
          </template>
        </el-table-column>
        <el-table-column label="等级" width="90">
          <template #default="{ row }">
            <el-tag size="small" :type="EVENT_LEVEL_TAG[row.level]">
              {{ EVENT_LEVEL_LABELS[row.level] }}
            </el-tag>
          </template>
        </el-table-column>
        <el-table-column prop="description" label="描述" min-width="240" show-overflow-tooltip />
        <el-table-column label="调度建议" min-width="200" show-overflow-tooltip>
          <template #default="{ row }">{{ row.suggestion || '—' }}</template>
        </el-table-column>
        <el-table-column label="状态" width="100">
          <template #default="{ row }">
            <el-tag size="small" :type="EVENT_STATUS_TAG[row.status]">
              {{ EVENT_STATUS_LABELS[row.status] }}
            </el-tag>
          </template>
        </el-table-column>
        <el-table-column label="操作" width="150" fixed="right">
          <template #default="{ row }">
            <template v-if="authStore.isAdmin">
              <el-button size="small" text type="primary" @click="handleConfirm(row)">
                确认
              </el-button>
              <el-button size="small" text type="danger" @click="handleClose(row)">
                关闭
              </el-button>
            </template>
            <span v-else class="muted">—</span>
          </template>
        </el-table-column>
      </el-table>
    </el-card>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import StatCard from '@/components/StatCard.vue'
import { useAuthStore } from '@/stores/auth'
import { usePolling } from '@/composables/usePolling'
import { getEvents, updateEventStatus } from '@/api'
import type { EventItem, EventLevel, EventType } from '@/api'
import {
  EVENT_LEVEL_LABELS,
  EVENT_LEVEL_TAG,
  EVENT_TYPE_LABELS,
  EVENT_STATUS_LABELS,
  EVENT_STATUS_TAG,
} from '@/utils/status'

const authStore = useAuthStore()

/** 告警类型集合 */
const ALERT_TYPES: EventType[] = ['CONGESTION', 'OVERLOAD', 'PARKING_SHORTAGE', 'CAPACITY_SHORTAGE']
/** 告警状态集合（未闭环） */
const ALERT_STATUSES: string[] = ['ACTIVE', 'ACKNOWLEDGED']
const LEVEL_OPTIONS: EventLevel[] = ['CRITICAL', 'HIGH', 'WARNING', 'INFO']

const alerts = ref<EventItem[]>([])
const filterLevel = ref('')
const filterType = ref('')

/** 客户端过滤：仅保留告警类型且未闭环的状态 */
async function fetch() {
  try {
    const res = await getEvents()
    alerts.value = res.data.filter(
      (e) => ALERT_TYPES.includes(e.type) && ALERT_STATUSES.includes(e.status),
    )
  } catch (e) {}
}

usePolling(fetch, 3000)

const filteredAlerts = computed(() =>
  alerts.value.filter(
    (e) => (!filterLevel.value || e.level === filterLevel.value) &&
      (!filterType.value || e.type === filterType.value),
  ),
)

const levelCounts = computed(() => {
  const counts: Record<EventLevel, number> = { CRITICAL: 0, HIGH: 0, WARNING: 0, INFO: 0 }
  for (const e of alerts.value) counts[e.level]++
  return counts
})

/** 紧急告警整行淡红底 */
function rowClassName({ row }: { row: EventItem }) {
  return row.level === 'CRITICAL' ? 'critical-row' : ''
}

async function handleConfirm(row: EventItem) {
  try {
    const res = await updateEventStatus(row.eventId, 'ACKNOWLEDGED')
    if (res.code !== 200) {
      ElMessage.error(res.message)
      return
    }
    ElMessage.success('操作成功')
    await fetch()
  } catch (e) {}
}

async function handleClose(row: EventItem) {
  try {
    await ElMessageBox.confirm('确认关闭该告警？', '提示', { type: 'warning' })
  } catch (e) {
    return
  }
  try {
    const res = await updateEventStatus(row.eventId, 'CLOSED')
    if (res.code !== 200) {
      ElMessage.error(res.message)
      return
    }
    ElMessage.success('操作成功')
    await fetch()
  } catch (e) {}
}
</script>

<style scoped>
.alerts-page {
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

.toolbar {
  display: flex;
  gap: 12px;
  flex-wrap: wrap;
}

.table-card {
  border-radius: 6px;
}

.card-header {
  font-size: 15px;
  font-weight: 600;
  color: #303133;
}

.muted {
  color: #909399;
}

:deep(.critical-row) {
  background: #fef0f0 !important;
}

@media (max-width: 768px) {
  .stat-col {
    flex: 1 1 100%;
  }
}
</style>
