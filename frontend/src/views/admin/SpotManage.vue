<template>
  <div class="spot-manage-page">
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
          <span>景点列表</span>
          <el-button v-if="authStore.isAdmin" type="primary" size="small" @click="openAdd">新增景点</el-button>
        </div>
      </template>

      <el-table :data="spots" stripe>
        <el-table-column prop="spotId" label="景点ID" width="110" />
        <el-table-column prop="name" label="名称" min-width="120" />
        <el-table-column prop="capacity" label="瞬时容量" width="100" />
        <el-table-column prop="dailyCapacity" label="全日容量" width="100" />
        <el-table-column label="当前在园" min-width="180">
          <template #default="{ row }">
            <el-progress
              :percentage="visitorPercentage(row)"
              :color="getLevelColor(row.level)"
              :stroke-width="12"
              :text-inside="true"
              :format="() => `${row.currentVisitors} / ${row.capacity}`"
            />
          </template>
        </el-table-column>
        <el-table-column prop="totalIn" label="累计入园" width="100" />
        <el-table-column label="拥挤等级" width="110">
          <template #default="{ row }">
            <LevelTag :level="row.level" />
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
      :title="dialogMode === 'add' ? '新增景点' : '编辑景点'"
      width="480px"
    >
      <el-form :model="form" label-width="90px">
        <el-form-item label="名称">
          <el-input v-model="form.name" placeholder="请输入景点名称" />
        </el-form-item>
        <el-form-item label="瞬时容量">
          <el-input-number v-model="form.capacity" :min="0" style="width: 100%" />
        </el-form-item>
        <el-form-item label="全日容量">
          <el-input-number v-model="form.dailyCapacity" :min="0" style="width: 100%" />
        </el-form-item>
        <el-form-item label="当前在园">
          <el-input-number v-model="form.currentVisitors" :min="0" style="width: 100%" />
        </el-form-item>
        <el-form-item label="坐标X">
          <el-input-number v-model="form.locationX" :min="0" style="width: 100%" />
        </el-form-item>
        <el-form-item label="坐标Y">
          <el-input-number v-model="form.locationY" :min="0" style="width: 100%" />
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
import LevelTag from '@/components/LevelTag.vue'
import { useAuthStore } from '@/stores/auth'
import { usePolling } from '@/composables/usePolling'
import { getSpots } from '@/api'
import type { Spot } from '@/api'
import { getLevelColor, ratioToLevel } from '@/utils/level'

const authStore = useAuthStore()
const spots = ref<Spot[]>([])

async function fetchSpots() {
  try {
    const res = await getSpots()
    spots.value = res.data
  } catch (e) {}
}

// 定时刷新（5 秒）
usePolling(fetchSpots, 5000)

const dialogVisible = ref(false)
const dialogMode = ref<'add' | 'edit'>('add')
const editingId = ref<string | null>(null)

const form = reactive({
  name: '',
  capacity: 0,
  dailyCapacity: 0,
  currentVisitors: 0,
  locationX: 0,
  locationY: 0,
})

/** 当前在园占用百分比 */
function visitorPercentage(row: Spot): number {
  if (!row.capacity) return 0
  return Math.min(100, Math.round((row.currentVisitors / row.capacity) * 100))
}

function resetForm() {
  form.name = ''
  form.capacity = 0
  form.dailyCapacity = 0
  form.currentVisitors = 0
  form.locationX = 0
  form.locationY = 0
}

function openAdd() {
  dialogMode.value = 'add'
  editingId.value = null
  resetForm()
  dialogVisible.value = true
}

function openEdit(row: Spot) {
  dialogMode.value = 'edit'
  editingId.value = row.spotId
  form.name = row.name
  form.capacity = row.capacity
  form.dailyCapacity = row.dailyCapacity
  form.currentVisitors = row.currentVisitors
  form.locationX = row.locationX
  form.locationY = row.locationY
  dialogVisible.value = true
}

function handleSave() {
  if (!form.name) {
    ElMessage.warning('请输入景点名称')
    return
  }
  // TODO: 后端景点管理接口就绪后，替换为 createSpot / updateSpot / deleteSpot 调用
  const level = ratioToLevel(form.capacity ? form.currentVisitors / form.capacity : 0)
  if (dialogMode.value === 'add') {
    spots.value.push({
      spotId: `S${Date.now()}`,
      name: form.name,
      capacity: form.capacity,
      dailyCapacity: form.dailyCapacity,
      currentVisitors: form.currentVisitors,
      totalIn: 0,
      arrivalRate: 0,
      departureRate: 0,
      locationX: form.locationX,
      locationY: form.locationY,
      level,
    })
  } else if (editingId.value) {
    const target = spots.value.find((s) => s.spotId === editingId.value)
    if (target) {
      target.name = form.name
      target.capacity = form.capacity
      target.dailyCapacity = form.dailyCapacity
      target.currentVisitors = form.currentVisitors
      target.locationX = form.locationX
      target.locationY = form.locationY
      target.level = level
    }
  }
  ElMessage.success('操作成功')
  dialogVisible.value = false
}

async function handleDelete(row: Spot) {
  try {
    await ElMessageBox.confirm(`确定删除景点「${row.name}」吗？`, '提示', {
      confirmButtonText: '确定',
      cancelButtonText: '取消',
      type: 'warning',
    })
  } catch (e) {
    return
  }
  // TODO: 后端景点管理接口就绪后，替换为 createSpot / updateSpot / deleteSpot 调用
  spots.value = spots.value.filter((s) => s.spotId !== row.spotId)
  ElMessage.success('操作成功')
}
</script>

<style scoped>
.spot-manage-page {
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
