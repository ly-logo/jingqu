<template>
  <div class="route-page">
    <!-- 说明提示 -->
    <el-alert
      type="info"
      :closable="false"
      title="距离矩阵用于游客分流与摆渡车调配的距离计算，修改后即时生效"
    />

    <el-card shadow="never" class="table-card">
      <template #header>
        <div class="card-header">
          <span>距离矩阵（单位：公里）</span>
          <el-button v-if="authStore.isAdmin" size="small" @click="handleReset">重置为默认矩阵</el-button>
        </div>
      </template>

      <el-table :data="matrix" border>
        <el-table-column label="站点＼距离(公里)" width="150">
          <template #default="{ $index }">
            <span class="row-station">{{ stations[$index] }}</span>
          </template>
        </el-table-column>

        <el-table-column
          v-for="(col, j) in stations"
          :key="j"
          :label="col"
          align="center"
          min-width="90"
        >
          <template #default="{ row, $index }">
            <!-- 管理员可编辑单元格 -->
            <el-input-number
              v-if="isEditing($index, j)"
              :ref="setCellInput"
              v-model="draft"
              :min="0"
              :max="10"
              :step="0.1"
              :precision="1"
              size="small"
              controls-position="right"
              style="width: 100%"
              @blur="saveCell"
              @keyup.enter="saveCell"
            />
            <span
              v-else
              class="cell-value"
              :class="{ 'cell-editable': authStore.isAdmin && $index !== j }"
              @click="startEdit($index, j)"
            >
              {{ displayDistance(row, $index, j) }}
            </span>
          </template>
        </el-table-column>
      </el-table>
    </el-card>
  </div>
</template>

<script setup lang="ts">
import { ref, nextTick } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { useAuthStore } from '@/stores/auth'

const authStore = useAuthStore()

/** 7 个站点 */
const stations = ['东门', '索道站', '观瀑台', '百花谷', '望湖亭', '青石古镇', '山顶广场']

/** 默认距离矩阵（对称，对角线为 0，单位：公里） */
const DEFAULT_MATRIX: number[][] = [
  [0, 1.2, 2.0, 2.6, 3.0, 2.2, 3.5],
  [1.2, 0, 0.8, 1.6, 2.4, 1.4, 2.8],
  [2.0, 0.8, 0, 0.9, 1.8, 2.0, 2.6],
  [2.6, 1.6, 0.9, 0, 1.1, 1.5, 2.1],
  [3.0, 2.4, 1.8, 1.1, 0, 1.3, 1.7],
  [2.2, 1.4, 2.0, 1.5, 1.3, 0, 1.2],
  [3.5, 2.8, 2.6, 2.1, 1.7, 1.2, 0],
]

const matrix = ref<number[][]>(DEFAULT_MATRIX.map((row) => [...row]))

const editing = ref<{ i: number; j: number } | null>(null)
const draft = ref(0)
const cellInput = ref<any>(null)

function setCellInput(el: any) {
  cellInput.value = el
}

function isEditing(i: number, j: number): boolean {
  return !!editing.value && editing.value.i === i && editing.value.j === j
}

/** 对角线显示 '—'，其余显示一位小数 */
function displayDistance(row: number[], i: number, j: number): string {
  if (i === j) return '—'
  return row[j].toFixed(1)
}

function startEdit(i: number, j: number) {
  if (!authStore.isAdmin || i === j) return
  editing.value = { i, j }
  draft.value = matrix.value[i][j]
  nextTick(() => {
    cellInput.value?.focus?.()
  })
}

/** 失焦或回车保存，并同步对称项 */
function saveCell() {
  if (!editing.value) return
  const { i, j } = editing.value
  // TODO: 后端距离矩阵接口就绪后，替换为保存距离矩阵的接口调用
  matrix.value[i][j] = draft.value
  matrix.value[j][i] = draft.value
  editing.value = null
}

async function handleReset() {
  try {
    await ElMessageBox.confirm('确定重置为默认距离矩阵吗？', '提示', {
      confirmButtonText: '确定',
      cancelButtonText: '取消',
      type: 'warning',
    })
  } catch (e) {
    return
  }
  matrix.value = DEFAULT_MATRIX.map((row) => [...row])
  ElMessage.success('操作成功')
}
</script>

<style scoped>
.route-page {
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

.row-station {
  font-weight: 600;
  color: #303133;
  font-size: 13px;
}

.cell-value {
  display: inline-block;
  min-width: 56px;
  font-size: 13px;
  color: #606266;
}

.cell-editable {
  cursor: pointer;
  border-radius: 6px;
  padding: 2px 6px;
  transition: background-color 0.2s;
}

.cell-editable:hover {
  background-color: #ecf5ff;
  color: #409eff;
}
</style>
