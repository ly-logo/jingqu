<template>
  <div class="rules-page">
    <div class="toolbar">
      <el-button v-if="authStore.isAdmin" type="primary" @click="openCreate">新增规则</el-button>
    </div>

    <el-card shadow="never" class="table-card">
      <el-table :data="rules" stripe>
        <el-table-column prop="ruleId" label="规则ID" width="90" />
        <el-table-column label="适用景点" min-width="140">
          <template #default="{ row }">{{ spotLabel(row.spotId) }}</template>
        </el-table-column>
        <el-table-column label="指标" min-width="170">
          <template #default="{ row }">{{ RULE_METRIC_LABELS[row.metric] }}</template>
        </el-table-column>
        <el-table-column label="条件" width="120">
          <template #default="{ row }">{{ row.operator }} {{ row.threshold }}</template>
        </el-table-column>
        <el-table-column prop="action" label="动作" min-width="200" show-overflow-tooltip />
        <el-table-column label="等级" width="100">
          <template #default="{ row }">
            <el-tag :type="EVENT_LEVEL_TAG[row.level]" size="small">
              {{ EVENT_LEVEL_LABELS[row.level] }}
            </el-tag>
          </template>
        </el-table-column>
        <el-table-column prop="priority" label="优先级" width="90" />
        <el-table-column label="启用" width="90">
          <template #default="{ row }">
            <el-switch
              :model-value="row.enabled"
              :disabled="!authStore.isAdmin"
              @change="(val) => toggleEnabled(row, val)"
            />
          </template>
        </el-table-column>
        <el-table-column label="操作" width="150">
          <template #default="{ row }">
            <el-button
              v-if="authStore.isAdmin"
              link
              type="primary"
              size="small"
              @click="openEdit(row)"
            >
              编辑
            </el-button>
            <el-button
              v-if="authStore.isAdmin"
              link
              type="danger"
              size="small"
              @click="handleDelete(row)"
            >
              删除
            </el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <el-dialog v-model="dialogVisible" :title="editId === null ? '新增规则' : '编辑规则'" width="560px">
      <el-form :model="form" label-width="110px">
        <el-form-item label="适用景点">
          <el-select v-model="form.spotId" style="width: 100%">
            <el-option label="全部景点" value="*" />
            <el-option
              v-for="s in spots"
              :key="s.spotId"
              :label="s.name"
              :value="s.spotId"
            />
          </el-select>
        </el-form-item>
        <el-form-item label="指标">
          <el-select v-model="form.metric" style="width: 100%">
            <el-option
              v-for="k in metricOptions"
              :key="k"
              :label="RULE_METRIC_LABELS[k]"
              :value="k"
            />
          </el-select>
        </el-form-item>
        <el-form-item label="运算符">
          <el-select v-model="form.operator" style="width: 100%">
            <el-option v-for="op in operatorOptions" :key="op" :label="op" :value="op" />
          </el-select>
        </el-form-item>
        <el-form-item label="阈值">
          <el-input-number v-model="form.threshold" :step="0.05" :precision="2" />
        </el-form-item>
        <el-form-item label="动作">
          <el-input v-model="form.action" placeholder="请输入触发动作描述" />
        </el-form-item>
        <el-form-item label="等级">
          <el-select v-model="form.level" style="width: 100%">
            <el-option
              v-for="k in levelOptions"
              :key="k"
              :label="EVENT_LEVEL_LABELS[k]"
              :value="k"
            />
          </el-select>
        </el-form-item>
        <el-form-item label="优先级">
          <el-input-number v-model="form.priority" :min="0" />
          <span class="hint">数值越小优先级越高</span>
        </el-form-item>
        <el-form-item label="启用">
          <el-switch v-model="form.enabled" />
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="dialogVisible = false">取消</el-button>
        <el-button type="primary" :loading="submitting" @click="handleSubmit">确定</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { getRules, getSpots, createRule, updateRule, deleteRule } from '@/api'
import type { Rule, RuleMetric, RuleOperator, EventLevel, Spot } from '@/api'
import {
  RULE_METRIC_LABELS,
  EVENT_LEVEL_LABELS,
  EVENT_LEVEL_TAG,
} from '@/utils/status'
import { useAuthStore } from '@/stores/auth'
import { usePolling } from '@/composables/usePolling'

const authStore = useAuthStore()

const rules = ref<Rule[]>([])
const spots = ref<Spot[]>([])

const dialogVisible = ref(false)
const submitting = ref(false)
const editId = ref<number | null>(null)

const emptyForm = (): Omit<Rule, 'ruleId'> => ({
  spotId: '*',
  metric: 'visitor_ratio',
  operator: '>=',
  threshold: 0.9,
  action: '',
  level: 'HIGH',
  priority: 10,
  enabled: true,
})
const form = ref<Omit<Rule, 'ruleId'>>(emptyForm())

const metricOptions = Object.keys(RULE_METRIC_LABELS) as RuleMetric[]
const levelOptions = Object.keys(EVENT_LEVEL_LABELS) as EventLevel[]
const operatorOptions: RuleOperator[] = ['>', '>=', '<', '<=']

const spotNameMap = computed(() => {
  const map: Record<string, string> = {}
  for (const s of spots.value) map[s.spotId] = s.name
  return map
})

function spotLabel(spotId: string): string {
  if (spotId === '*') return '全部景点'
  return spotNameMap.value[spotId] || spotId
}

async function fetch() {
  try {
    const [ruleRes, spotRes] = await Promise.all([getRules(), getSpots()])
    rules.value = ruleRes.data
    spots.value = spotRes.data
  } catch {
    // silent in polling
  }
}

usePolling(fetch, 10000)

function openCreate() {
  editId.value = null
  form.value = emptyForm()
  dialogVisible.value = true
}

function openEdit(row: Rule) {
  editId.value = row.ruleId
  form.value = {
    spotId: row.spotId,
    metric: row.metric,
    operator: row.operator,
    threshold: row.threshold,
    action: row.action,
    level: row.level,
    priority: row.priority,
    enabled: row.enabled,
  }
  dialogVisible.value = true
}

async function handleSubmit() {
  if (!form.value.action) {
    ElMessage.warning('请填写触发动作描述')
    return
  }
  submitting.value = true
  try {
    const res =
      editId.value === null
        ? await createRule({ ...form.value })
        : await updateRule(editId.value, { ...form.value })
    if (res.code !== 200) {
      ElMessage.error(res.message)
      return
    }
    ElMessage.success(editId.value === null ? '新增成功' : '修改成功')
    dialogVisible.value = false
    fetch()
  } catch {
    // silent
  } finally {
    submitting.value = false
  }
}

async function toggleEnabled(row: Rule, val: string | number | boolean) {
  try {
    const res = await updateRule(row.ruleId, { enabled: Boolean(val) })
    if (res.code !== 200) {
      ElMessage.error(res.message)
      return
    }
    ElMessage.success('已更新')
    fetch()
  } catch {
    // silent
  }
}

async function handleDelete(row: Rule) {
  try {
    await ElMessageBox.confirm(`确认删除规则「${row.ruleId}」？`, '提示', {
      type: 'warning',
      confirmButtonText: '确认删除',
      cancelButtonText: '取消',
    })
  } catch {
    return
  }
  try {
    const res = await deleteRule(row.ruleId)
    if (res.code !== 200) {
      ElMessage.error(res.message)
      return
    }
    ElMessage.success('删除成功')
    fetch()
  } catch {
    // silent
  }
}
</script>

<style scoped>
.rules-page {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.toolbar {
  display: flex;
  justify-content: flex-end;
}

.table-card {
  border-radius: 6px;
}

.hint {
  margin-left: 8px;
  font-size: 13px;
  color: #909399;
}
</style>
