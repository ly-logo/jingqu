<template>
  <div class="events-page">
    <!-- 筛选 -->
    <div class="toolbar">
      <el-select v-model="filterType" placeholder="类型筛选" style="width: 160px">
        <el-option label="全部" value="" />
        <el-option
          v-for="type in TYPE_OPTIONS"
          :key="type"
          :label="EVENT_TYPE_LABELS[type]"
          :value="type"
        />
      </el-select>
      <el-select v-model="filterLevel" placeholder="等级筛选" style="width: 140px">
        <el-option label="全部" value="" />
        <el-option
          v-for="level in LEVEL_OPTIONS"
          :key="level"
          :label="EVENT_LEVEL_LABELS[level]"
          :value="level"
        />
      </el-select>
      <el-select v-model="filterStatus" placeholder="状态筛选" style="width: 140px">
        <el-option label="全部" value="" />
        <el-option
          v-for="status in STATUS_OPTIONS"
          :key="status"
          :label="EVENT_STATUS_LABELS[status]"
          :value="status"
        />
      </el-select>
    </div>

    <!-- 事件表格 -->
    <el-card shadow="never" class="table-card">
      <template #header>
        <div class="card-header">历史事件</div>
      </template>
      <el-table :data="pagedEvents" style="width: 100%">
        <el-table-column prop="eventId" label="事件ID" width="100" />
        <el-table-column prop="createdAt" label="时间" width="170" />
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
        <el-table-column prop="spotName" label="景点" width="120" />
        <el-table-column prop="description" label="描述" min-width="220" show-overflow-tooltip />
        <el-table-column label="状态" width="100">
          <template #default="{ row }">
            <el-tag size="small" :type="EVENT_STATUS_TAG[row.status]">
              {{ EVENT_STATUS_LABELS[row.status] }}
            </el-tag>
          </template>
        </el-table-column>
        <el-table-column label="更新时间" width="170">
          <template #default="{ row }">{{ row.updatedAt || '—' }}</template>
        </el-table-column>
        <el-table-column label="操作" width="260" fixed="right">
          <template #default="{ row }">
            <el-button size="small" text type="primary" @click="openDetail(row)">详情</el-button>
            <template v-if="authStore.isAdmin">
              <el-button size="small" text type="primary" @click="changeStatus(row, 'ACKNOWLEDGED')">
                确认
              </el-button>
              <el-button
                size="small"
                text
                type="success"
                @click="handleWithConfirm(row, 'RESOLVED', '确认恢复该事件？')"
              >
                恢复
              </el-button>
              <el-button
                size="small"
                text
                type="danger"
                @click="handleWithConfirm(row, 'CLOSED', '确认关闭该事件？')"
              >
                关闭
              </el-button>
              <el-button
                size="small"
                text
                type="info"
                @click="handleWithConfirm(row, 'IGNORED', '确认忽略该事件？')"
              >
                忽略
              </el-button>
            </template>
          </template>
        </el-table-column>
      </el-table>

      <div class="pagination-wrap">
        <el-pagination
          v-model:current-page="page"
          :page-size="PAGE_SIZE"
          :total="filteredEvents.length"
          layout="prev, pager, next, total"
          background
        />
      </div>
    </el-card>

    <!-- 详情抽屉 -->
    <el-drawer v-model="drawerVisible" title="事件详情" size="480px">
      <div v-if="detail" class="detail-list">
        <div class="detail-row">
          <span class="detail-label">事件ID</span>
          <span class="detail-value">{{ detail.eventId }}</span>
        </div>
        <div class="detail-row">
          <span class="detail-label">类型</span>
          <span class="detail-value">{{ EVENT_TYPE_LABELS[detail.type] }}</span>
        </div>
        <div class="detail-row">
          <span class="detail-label">等级</span>
          <span class="detail-value">
            <el-tag size="small" :type="EVENT_LEVEL_TAG[detail.level]">
              {{ EVENT_LEVEL_LABELS[detail.level] }}
            </el-tag>
          </span>
        </div>
        <div class="detail-row">
          <span class="detail-label">状态</span>
          <span class="detail-value">
            <el-tag size="small" :type="EVENT_STATUS_TAG[detail.status]">
              {{ EVENT_STATUS_LABELS[detail.status] }}
            </el-tag>
          </span>
        </div>
        <div class="detail-row">
          <span class="detail-label">景点</span>
          <span class="detail-value">{{ detail.spotName || '—' }}</span>
        </div>
        <div class="detail-row">
          <span class="detail-label">描述</span>
          <span class="detail-value">{{ detail.description }}</span>
        </div>
        <div class="detail-row">
          <span class="detail-label">调度建议</span>
          <span class="detail-value">{{ detail.suggestion || '—' }}</span>
        </div>
        <div class="detail-row">
          <span class="detail-label">生成时间</span>
          <span class="detail-value">{{ detail.createdAt }}</span>
        </div>
        <div class="detail-row">
          <span class="detail-label">更新时间</span>
          <span class="detail-value">{{ detail.updatedAt || '—' }}</span>
        </div>
      </div>
    </el-drawer>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, watch } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { useAuthStore } from '@/stores/auth'
import { usePolling } from '@/composables/usePolling'
import { getEvents, updateEventStatus } from '@/api'
import type { EventItem, EventLevel, EventType, EventStatus } from '@/api'
import {
  EVENT_LEVEL_LABELS,
  EVENT_LEVEL_TAG,
  EVENT_TYPE_LABELS,
  EVENT_STATUS_LABELS,
  EVENT_STATUS_TAG,
} from '@/utils/status'

const authStore = useAuthStore()

const TYPE_OPTIONS: EventType[] = [
  'CONGESTION',
  'OVERLOAD',
  'PARKING_SHORTAGE',
  'CAPACITY_SHORTAGE',
  'DATA_ANOMALY',
  'SYSTEM_ERROR',
]
const LEVEL_OPTIONS: EventLevel[] = ['CRITICAL', 'HIGH', 'WARNING', 'INFO']
const STATUS_OPTIONS: EventStatus[] = ['ACTIVE', 'ACKNOWLEDGED', 'RESOLVED', 'CLOSED', 'IGNORED']

const PAGE_SIZE = 10

const events = ref<EventItem[]>([])
const filterType = ref('')
const filterLevel = ref('')
const filterStatus = ref('')
const page = ref(1)
const drawerVisible = ref(false)
const detail = ref<EventItem | null>(null)

async function fetch() {
  try {
    const res = await getEvents()
    events.value = res.data
  } catch (e) {}
}

usePolling(fetch, 5000)

const filteredEvents = computed(() =>
  events.value.filter(
    (e) => (!filterType.value || e.type === filterType.value) &&
      (!filterLevel.value || e.level === filterLevel.value) &&
      (!filterStatus.value || e.status === filterStatus.value),
  ),
)

const pagedEvents = computed(() =>
  filteredEvents.value.slice((page.value - 1) * PAGE_SIZE, page.value * PAGE_SIZE),
)

watch([filterType, filterLevel, filterStatus], () => {
  page.value = 1
})

function openDetail(row: EventItem) {
  detail.value = row
  drawerVisible.value = true
}

async function changeStatus(row: EventItem, status: EventStatus) {
  try {
    const res = await updateEventStatus(row.eventId, status)
    if (res.code !== 200) {
      ElMessage.error(res.message)
      return
    }
    ElMessage.success('操作成功')
    await fetch()
  } catch (e) {}
}

async function handleWithConfirm(row: EventItem, status: EventStatus, text: string) {
  try {
    await ElMessageBox.confirm(text, '提示', { type: 'warning' })
  } catch (e) {
    return
  }
  await changeStatus(row, status)
}
</script>

<style scoped>
.events-page {
  display: flex;
  flex-direction: column;
  gap: 16px;
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

.pagination-wrap {
  display: flex;
  justify-content: flex-end;
  margin-top: 16px;
}

.detail-list {
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.detail-row {
  display: flex;
  align-items: flex-start;
  gap: 12px;
  font-size: 13px;
}

.detail-label {
  width: 72px;
  flex-shrink: 0;
  color: #909399;
}

.detail-value {
  flex: 1;
  color: #303133;
  word-break: break-all;
}
</style>
