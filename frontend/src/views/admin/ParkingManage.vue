<template>
  <div class="parking-manage-page">
    <!-- 只读提示 -->
    <el-alert
      v-if="!authStore.isAdmin"
      type="info"
      :closable="false"
      title="当前为只读视角，如需修改请联系管理员"
    />

    <el-card shadow="never" class="table-card">
      <template #header>
        <div class="card-header">
          <span>停车场列表</span>
          <el-button v-if="authStore.isAdmin" type="primary" size="small" @click="openAdd">新增停车场</el-button>
        </div>
      </template>

      <el-table :data="parkings" stripe>
        <el-table-column prop="parkingId" label="停车场ID" width="110" />
        <el-table-column prop="name" label="名称" min-width="120" />
        <el-table-column prop="totalSpaces" label="总车位" width="90" />
        <el-table-column prop="occupiedSpaces" label="已占用" width="90" />
        <el-table-column prop="freeSpaces" label="剩余车位" width="90" />
        <el-table-column label="剩余率" min-width="180">
          <template #default="{ row }">
            <el-progress
              :percentage="freeRate(row)"
              :color="freeColor(row)"
              :stroke-width="12"
              :text-inside="true"
            />
          </template>
        </el-table-column>
        <el-table-column v-if="authStore.isAdmin" label="操作" width="140" fixed="right">
          <template #default="{ row }">
            <el-button link type="primary" size="small" @click="openEdit(row)">编辑</el-button>
            <el-button link type="danger" size="small" @click="handleDelete(row)">删除</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <el-dialog
      v-model="dialogVisible"
      :title="dialogMode === 'add' ? '新增停车场' : '编辑停车场'"
      width="480px"
    >
      <el-form :model="form" label-width="90px">
        <el-form-item label="名称">
          <el-input v-model="form.name" placeholder="请输入停车场名称" />
        </el-form-item>
        <el-form-item label="总车位">
          <el-input-number v-model="form.totalSpaces" :min="0" style="width: 100%" />
        </el-form-item>
        <el-form-item label="已占用">
          <el-input-number v-model="form.occupiedSpaces" :min="0" :max="form.totalSpaces" style="width: 100%" />
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="dialogVisible = false">取消</el-button>
        <el-button type="primary" @click="handleSave">确定</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup lang="ts">
import { ref, reactive } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { useAuthStore } from '@/stores/auth'
import { usePolling } from '@/composables/usePolling'
import { getParkings } from '@/api'
import type { ParkingLot } from '@/api'

const authStore = useAuthStore()
const parkings = ref<ParkingLot[]>([])

async function fetchParkings() {
  try {
    const res = await getParkings()
    parkings.value = res.data
  } catch (e) {}
}

// 定时刷新（5 秒）
usePolling(fetchParkings, 5000)

const dialogVisible = ref(false)
const dialogMode = ref<'add' | 'edit'>('add')
const editingId = ref<string | null>(null)

const form = reactive({
  name: '',
  totalSpaces: 0,
  occupiedSpaces: 0,
})

/** 剩余率百分比 */
function freeRate(row: ParkingLot): number {
  if (!row.totalSpaces) return 0
  return Math.round((row.freeSpaces / row.totalSpaces) * 100)
}

/** 剩余率配色：<10% 红、<30% 橙、否则绿 */
function freeColor(row: ParkingLot): string {
  const rate = freeRate(row)
  if (rate < 10) return '#F56C6C'
  if (rate < 30) return '#E6A23C'
  return '#67C23A'
}

function resetForm() {
  form.name = ''
  form.totalSpaces = 0
  form.occupiedSpaces = 0
}

function openAdd() {
  dialogMode.value = 'add'
  editingId.value = null
  resetForm()
  dialogVisible.value = true
}

function openEdit(row: ParkingLot) {
  dialogMode.value = 'edit'
  editingId.value = row.parkingId
  form.name = row.name
  form.totalSpaces = row.totalSpaces
  form.occupiedSpaces = row.occupiedSpaces
  dialogVisible.value = true
}

function handleSave() {
  if (!form.name) {
    ElMessage.warning('请输入停车场名称')
    return
  }
  // TODO: 后端停车场管理接口就绪后，替换为对应的 create / update / delete 调用
  const freeSpaces = Math.max(0, form.totalSpaces - form.occupiedSpaces)
  if (dialogMode.value === 'add') {
    parkings.value.push({
      parkingId: `P${Date.now()}`,
      name: form.name,
      totalSpaces: form.totalSpaces,
      occupiedSpaces: form.occupiedSpaces,
      freeSpaces,
    })
  } else if (editingId.value) {
    const target = parkings.value.find((p) => p.parkingId === editingId.value)
    if (target) {
      target.name = form.name
      target.totalSpaces = form.totalSpaces
      target.occupiedSpaces = form.occupiedSpaces
      target.freeSpaces = freeSpaces
    }
  }
  ElMessage.success('操作成功')
  dialogVisible.value = false
}

async function handleDelete(row: ParkingLot) {
  try {
    await ElMessageBox.confirm(`确定删除停车场「${row.name}」吗？`, '提示', {
      confirmButtonText: '确定',
      cancelButtonText: '取消',
      type: 'warning',
    })
  } catch (e) {
    return
  }
  // TODO: 后端停车场管理接口就绪后，替换为对应的 create / update / delete 调用
  parkings.value = parkings.value.filter((p) => p.parkingId !== row.parkingId)
  ElMessage.success('操作成功')
}
</script>

<style scoped>
.parking-manage-page {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.table-card {
  border-radius: 6px;
}

.card-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  font-size: 15px;
  font-weight: 600;
  color: #303133;
}
</style>
