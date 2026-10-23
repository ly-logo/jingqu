<template>
  <div class="data-entry-page">
    <el-alert
      v-if="simRunning"
      type="warning"
      :closable="false"
      show-icon
      title="模拟数据生成器运行中，手动录入已禁用"
    />

    <div class="entry-grid">
      <el-card shadow="never" class="entry-form-card">
        <template #header>
          <div class="card-header">手动数据录入</div>
        </template>

        <el-tabs v-model="activeTab">
          <el-tab-pane label="景点流量录入" name="visitor">
            <el-form :model="visitorForm" label-width="120px">
              <el-form-item label="景点">
                <el-select v-model="visitorForm.spotId" placeholder="请选择景点" style="width: 100%">
                  <el-option
                    v-for="s in spots"
                    :key="s.spotId"
                    :label="s.name"
                    :value="s.spotId"
                  />
                </el-select>
              </el-form-item>
              <el-form-item label="当前在园人数">
                <el-input-number
                  v-model="visitorForm.currentVisitors"
                  :min="0"
                  :disabled="simRunning"
                />
              </el-form-item>
              <el-form-item>
                <el-button
                  type="primary"
                  :disabled="simRunning"
                  :loading="visitorSubmitting"
                  @click="submitVisitor"
                >
                  提交
                </el-button>
              </el-form-item>
            </el-form>
          </el-tab-pane>

          <el-tab-pane label="停车场录入" name="parking">
            <el-form :model="parkingForm" label-width="120px">
              <el-form-item label="停车场">
                <el-select
                  v-model="parkingForm.parkingId"
                  placeholder="请选择停车场"
                  style="width: 100%"
                >
                  <el-option
                    v-for="p in parkings"
                    :key="p.parkingId"
                    :label="p.name"
                    :value="p.parkingId"
                  />
                </el-select>
              </el-form-item>
              <el-form-item label="已占用车位数">
                <el-input-number
                  v-model="parkingForm.occupiedSpaces"
                  :min="0"
                  :max="maxSpaces"
                  :disabled="simRunning"
                />
                <span v-if="selectedParking" class="unit">上限 {{ selectedParking.totalSpaces }}</span>
              </el-form-item>
              <el-form-item>
                <el-button
                  type="primary"
                  :disabled="simRunning"
                  :loading="parkingSubmitting"
                  @click="submitParking"
                >
                  提交
                </el-button>
              </el-form-item>
            </el-form>
          </el-tab-pane>
        </el-tabs>
      </el-card>

      <el-card shadow="never" class="entry-recent-card">
        <template #header>
          <div class="card-header">最近录入</div>
        </template>
        <el-empty v-if="recent.length === 0" description="暂无录入记录" />
        <div v-else class="recent-list">
          <div v-for="(r, i) in recent" :key="i" class="recent-item">
            <span class="recent-time">{{ r.time }}</span>
            <el-tag size="small" :type="r.kind === '景点流量' ? 'primary' : 'success'">
              {{ r.kind }}
            </el-tag>
            <span class="recent-summary">{{ r.summary }}</span>
          </div>
        </div>
      </el-card>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue'
import { ElMessage } from 'element-plus'
import {
  getSpots,
  getParkings,
  getSimulationState,
  submitVisitorEntry,
  submitParkingEntry,
} from '@/api'
import type { Spot, ParkingLot } from '@/api'
import { usePolling } from '@/composables/usePolling'

const activeTab = ref('visitor')
const simRunning = ref(false)
const spots = ref<Spot[]>([])
const parkings = ref<ParkingLot[]>([])

const visitorForm = ref({ spotId: '', currentVisitors: 0 })
const parkingForm = ref({ parkingId: '', occupiedSpaces: 0 })

const visitorSubmitting = ref(false)
const parkingSubmitting = ref(false)

interface RecentRecord {
  time: string
  kind: string
  summary: string
}
const recent = ref<RecentRecord[]>([])

const selectedParking = computed(() =>
  parkings.value.find((p) => p.parkingId === parkingForm.value.parkingId),
)
const maxSpaces = computed(() => selectedParking.value?.totalSpaces ?? 999999)

function nowText(): string {
  const d = new Date()
  const p = (n: number) => String(n).padStart(2, '0')
  return `${p(d.getHours())}:${p(d.getMinutes())}:${p(d.getSeconds())}`
}

function pushRecent(kind: string, summary: string) {
  recent.value.unshift({ time: nowText(), kind, summary })
  if (recent.value.length > 5) recent.value.pop()
}

async function refreshState() {
  try {
    const [stateRes, spotRes, parkingRes] = await Promise.all([
      getSimulationState(),
      getSpots(),
      getParkings(),
    ])
    simRunning.value = stateRes.data.running
    spots.value = spotRes.data
    parkings.value = parkingRes.data
  } catch {
    // silent in polling
  }
}

usePolling(refreshState, 5000)

async function submitVisitor() {
  if (!visitorForm.value.spotId) {
    ElMessage.warning('请选择景点')
    return
  }
  visitorSubmitting.value = true
  try {
    const res = await submitVisitorEntry({ ...visitorForm.value })
    if (res.code !== 200) {
      if (res.code === 409) {
        ElMessage.warning(res.message)
      } else {
        ElMessage.error(res.message)
      }
      return
    }
    ElMessage.success('录入成功')
    const name =
      spots.value.find((s) => s.spotId === visitorForm.value.spotId)?.name ||
      visitorForm.value.spotId
    pushRecent('景点流量', `${name} 在园 ${visitorForm.value.currentVisitors} 人`)
    visitorForm.value.currentVisitors = 0
  } catch {
    // 真实接口异常由拦截器处理
  } finally {
    visitorSubmitting.value = false
  }
}

async function submitParking() {
  if (!parkingForm.value.parkingId) {
    ElMessage.warning('请选择停车场')
    return
  }
  parkingSubmitting.value = true
  try {
    const res = await submitParkingEntry({ ...parkingForm.value })
    if (res.code !== 200) {
      if (res.code === 409) {
        ElMessage.warning(res.message)
      } else {
        ElMessage.error(res.message)
      }
      return
    }
    ElMessage.success('录入成功')
    const name =
      parkings.value.find((p) => p.parkingId === parkingForm.value.parkingId)?.name ||
      parkingForm.value.parkingId
    pushRecent('停车场', `${name} 已占用 ${parkingForm.value.occupiedSpaces} 位`)
    parkingForm.value.occupiedSpaces = 0
  } catch {
    // silent
  } finally {
    parkingSubmitting.value = false
  }
}
</script>

<style scoped>
.data-entry-page {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.entry-grid {
  display: grid;
  grid-template-columns: minmax(0, 1fr) 360px;
  gap: 16px;
}

.entry-form-card,
.entry-recent-card {
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

.recent-list {
  display: flex;
  flex-direction: column;
  gap: 10px;
}

.recent-item {
  display: flex;
  align-items: center;
  gap: 8px;
  font-size: 13px;
  color: #606266;
}

.recent-time {
  color: #909399;
  flex-shrink: 0;
}

.recent-summary {
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

@media (max-width: 992px) {
  .entry-grid {
    grid-template-columns: 1fr;
  }
}
</style>
