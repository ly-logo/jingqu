<template>
  <div class="scheduling-page">
    <!-- 筛选 -->
    <div class="toolbar">
      <el-select v-model="filterType" placeholder="类型筛选" style="width: 180px">
        <el-option label="全部" value="" />
        <el-option
          v-for="type in TYPE_OPTIONS"
          :key="type"
          :label="SUGGESTION_TYPE_LABELS[type]"
          :value="type"
        />
      </el-select>
      <el-select v-model="filterStatus" placeholder="状态筛选" style="width: 160px">
        <el-option label="全部" value="" />
        <el-option
          v-for="status in STATUS_OPTIONS"
          :key="status"
          :label="SUGGESTION_STATUS_LABELS[status]"
          :value="status"
        />
      </el-select>
    </div>

    <!-- 建议卡片流 -->
    <div class="suggestion-grid">
      <el-card
        v-for="item in filteredSuggestions"
        :key="item.suggestionId"
        shadow="never"
        class="suggestion-card"
      >
        <div class="card-head">
          <el-tag size="small">{{ SUGGESTION_TYPE_LABELS[item.type] }}</el-tag>
          <el-tag size="small" :type="SUGGESTION_STATUS_TAG[item.status]">
            {{ SUGGESTION_STATUS_LABELS[item.status] }}
          </el-tag>
        </div>

        <div class="card-body" @click="openDetail(item)">
          <div class="card-content">{{ item.content }}</div>
          <div v-if="item.detail" class="card-detail">{{ item.detail }}</div>
        </div>

        <div class="card-foot">
          <span class="card-time">{{ item.createdAt }}</span>
          <div v-if="authStore.isAdmin && item.status === 'PENDING'" class="card-actions">
            <el-button size="small" type="primary" @click="handleExecute(item)">执行</el-button>
            <el-button size="small" @click="handleReject(item)">驳回</el-button>
          </div>
        </div>
      </el-card>
    </div>

    <!-- 详情弹窗 -->
    <el-dialog v-model="detailVisible" title="调度建议详情" width="520px">
      <div v-if="detail" class="detail-list">
        <div class="detail-row">
          <span class="detail-label">编号</span>
          <span class="detail-value">{{ detail.suggestionId }}</span>
        </div>
        <div class="detail-row">
          <span class="detail-label">类型</span>
          <span class="detail-value">{{ SUGGESTION_TYPE_LABELS[detail.type] }}</span>
        </div>
        <div class="detail-row">
          <span class="detail-label">状态</span>
          <span class="detail-value">
            <el-tag size="small" :type="SUGGESTION_STATUS_TAG[detail.status]">
              {{ SUGGESTION_STATUS_LABELS[detail.status] }}
            </el-tag>
          </span>
        </div>
        <div class="detail-row">
          <span class="detail-label">内容</span>
          <span class="detail-value">{{ detail.content }}</span>
        </div>
        <div class="detail-row">
          <span class="detail-label">详情</span>
          <span class="detail-value">{{ detail.detail || '—' }}</span>
        </div>
        <div class="detail-row">
          <span class="detail-label">关联景点</span>
          <span class="detail-value">{{ detail.spotId || '—' }}</span>
        </div>
        <div class="detail-row">
          <span class="detail-label">生成时间</span>
          <span class="detail-value">{{ detail.createdAt }}</span>
        </div>
      </div>
    </el-dialog>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { useAuthStore } from '@/stores/auth'
import { usePolling } from '@/composables/usePolling'
import { getSuggestions, getSuggestionDetail, updateSuggestionStatus } from '@/api'
import type { Suggestion, SuggestionType, SuggestionStatus } from '@/api'
import {
  SUGGESTION_TYPE_LABELS,
  SUGGESTION_STATUS_LABELS,
  SUGGESTION_STATUS_TAG,
} from '@/utils/status'

const authStore = useAuthStore()

const TYPE_OPTIONS: SuggestionType[] = [
  'DIVERSION',
  'SHUTTLE_DISPATCH',
  'PARKING_GUIDE',
  'ENTRANCE_LIMIT',
  'RESOURCE_WARNING',
]
const STATUS_OPTIONS: SuggestionStatus[] = ['PENDING', 'EXECUTED', 'REJECTED']

const suggestions = ref<Suggestion[]>([])
const filterType = ref('')
const filterStatus = ref('')
const detailVisible = ref(false)
const detail = ref<Suggestion | null>(null)

async function fetch() {
  try {
    const res = await getSuggestions()
    suggestions.value = res.data
  } catch (e) {}
}

usePolling(fetch, 5000)

const filteredSuggestions = computed(() =>
  suggestions.value.filter(
    (s) => (!filterType.value || s.type === filterType.value) &&
      (!filterStatus.value || s.status === filterStatus.value),
  ),
)

async function openDetail(item: Suggestion) {
  try {
    const res = await getSuggestionDetail(item.suggestionId)
    detail.value = res.data
    detailVisible.value = true
  } catch (e) {}
}

async function handleExecute(item: Suggestion) {
  try {
    await ElMessageBox.confirm('确认执行该调度建议？', '提示', { type: 'warning' })
  } catch (e) {
    return
  }
  try {
    const res = await updateSuggestionStatus(item.suggestionId, 'EXECUTED')
    if (res.code !== 200) {
      ElMessage.error(res.message)
      return
    }
    ElMessage.success('操作成功')
    await fetch()
  } catch (e) {}
}

async function handleReject(item: Suggestion) {
  try {
    await ElMessageBox.confirm('确认驳回该调度建议？', '提示', { type: 'warning' })
  } catch (e) {
    return
  }
  try {
    const res = await updateSuggestionStatus(item.suggestionId, 'REJECTED')
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
.scheduling-page {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.toolbar {
  display: flex;
  gap: 12px;
  flex-wrap: wrap;
}

.suggestion-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(320px, 1fr));
  gap: 16px;
}

.suggestion-card {
  border-radius: 6px;
  display: flex;
  flex-direction: column;
}

.card-head {
  display: flex;
  align-items: center;
  justify-content: space-between;
  margin-bottom: 12px;
}

.card-body {
  flex: 1;
  cursor: pointer;
}

.card-content {
  font-size: 14px;
  font-weight: 600;
  color: #303133;
  line-height: 1.5;
}

.card-detail {
  font-size: 13px;
  color: #909399;
  margin-top: 6px;
  line-height: 1.5;
}

.card-foot {
  display: flex;
  align-items: center;
  justify-content: space-between;
  margin-top: 12px;
  padding-top: 12px;
  border-top: 1px solid #f0f2f5;
}

.card-time {
  font-size: 13px;
  color: #909399;
}

.card-actions {
  display: flex;
  gap: 8px;
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
