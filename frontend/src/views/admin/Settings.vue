<template>
  <div class="settings-page">
    <el-alert type="info" :closable="false" show-icon title="部分配置需重启服务后生效" />

    <el-card shadow="never" class="settings-card">
      <template #header>
        <div class="card-header">系统参数</div>
      </template>

      <el-form :model="form" label-width="180px">
        <el-form-item label="全景区总容量">
          <el-input-number v-model="form.totalCapacity" :min="1" />
          <span class="unit">人</span>
        </el-form-item>
        <el-form-item label="告警去抖窗口">
          <el-input-number v-model="form.debounceSeconds" :min="0" :max="300" />
          <span class="unit">秒</span>
          <div class="tip">用于避免同一告警在短时间内反复触发</div>
        </el-form-item>
        <el-form-item label="历史数据保留天数">
          <el-input-number v-model="form.retentionDays" :min="1" :max="365" />
          <span class="unit">天</span>
          <div class="tip">超期数据将被聚合或清理</div>
        </el-form-item>
        <el-form-item label="数据批量写库间隔">
          <el-input-number v-model="form.batchIntervalMs" :min="100" :max="10000" :step="100" />
          <span class="unit">毫秒</span>
        </el-form-item>
        <el-form-item>
          <el-button type="primary" :loading="saving" @click="handleSave">保存设置</el-button>
          <el-button @click="handleReset">恢复默认</el-button>
        </el-form-item>
      </el-form>
    </el-card>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { getSettings, updateSettings } from '@/api'
import type { SystemSettings } from '@/api'

const DEFAULT_SETTINGS: SystemSettings = {
  totalCapacity: 20000,
  debounceSeconds: 30,
  retentionDays: 7,
  batchIntervalMs: 1000,
}

const form = ref<SystemSettings>({ ...DEFAULT_SETTINGS })
const saving = ref(false)

async function fetch() {
  try {
    const res = await getSettings()
    form.value = { ...res.data }
  } catch {
    // silent
  }
}

onMounted(fetch)

async function handleSave() {
  saving.value = true
  try {
    const res = await updateSettings({ ...form.value })
    if (res.code !== 200) {
      ElMessage.error(res.message)
      return
    }
    ElMessage.success('设置已保存（部分配置重启后生效）')
  } catch {
    // silent
  } finally {
    saving.value = false
  }
}

async function handleReset() {
  try {
    await ElMessageBox.confirm('确认恢复默认设置？当前未保存的修改将被覆盖。', '提示', {
      type: 'warning',
      confirmButtonText: '恢复默认',
      cancelButtonText: '取消',
    })
  } catch {
    return
  }
  form.value = { ...DEFAULT_SETTINGS }
  ElMessage.success('已恢复默认值，请点击保存生效')
}
</script>

<style scoped>
.settings-page {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.settings-card {
  border-radius: 6px;
}

.card-header {
  font-size: 15px;
  font-weight: 600;
  color: #303133;
}

.unit {
  margin-left: 8px;
  font-size: 13px;
  color: #909399;
}

.tip {
  width: 100%;
  font-size: 12px;
  color: #909399;
  line-height: 1.4;
}
</style>
