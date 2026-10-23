<template>
  <div class="users-page">
    <div class="toolbar">
      <el-button type="primary" @click="openCreate">新增用户</el-button>
    </div>

    <el-card shadow="never" class="table-card">
      <el-table :data="users" stripe>
        <el-table-column prop="userId" label="用户ID" width="100" />
        <el-table-column prop="username" label="用户名" min-width="160" />
        <el-table-column label="角色" width="120">
          <template #default="{ row }">
            <el-tag :type="ROLE_TAG[row.role] || 'info'" size="small">
              {{ ROLE_LABELS[row.role] || row.role }}
            </el-tag>
          </template>
        </el-table-column>
        <el-table-column prop="createdAt" label="创建时间" min-width="180" />
        <el-table-column label="操作" width="100">
          <template #default="{ row }">
            <el-button link type="danger" size="small" @click="handleDelete(row)">删除</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <el-dialog v-model="dialogVisible" title="新增用户" width="480px">
      <el-form :model="form" label-width="90px">
        <el-form-item label="用户名">
          <el-input v-model="form.username" placeholder="请输入用户名" />
        </el-form-item>
        <el-form-item label="密码">
          <el-input v-model="form.password" type="password" show-password placeholder="请输入密码" />
        </el-form-item>
        <el-form-item label="角色">
          <el-select v-model="form.role" style="width: 100%">
            <el-option
              v-for="r in roleOptions"
              :key="r"
              :label="ROLE_LABELS[r]"
              :value="r"
            />
          </el-select>
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
import { ref } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { getUsers, createUser, deleteUser } from '@/api'
import type { User, UserCreateRequest } from '@/api'
import { ROLE_LABELS } from '@/utils/status'
import { usePolling } from '@/composables/usePolling'

const users = ref<User[]>([])
const dialogVisible = ref(false)
const submitting = ref(false)

const form = ref<UserCreateRequest>({ username: '', password: '', role: 'viewer' })

const roleOptions = ['admin', 'operator', 'viewer'] as const

const ROLE_TAG: Record<string, 'danger' | 'success' | 'info'> = {
  admin: 'danger',
  operator: 'success',
  viewer: 'info',
}

async function fetch() {
  try {
    const res = await getUsers()
    users.value = res.data
  } catch {
    // silent in polling
  }
}

usePolling(fetch, 10000)

function openCreate() {
  form.value = { username: '', password: '', role: 'viewer' }
  dialogVisible.value = true
}

async function handleSubmit() {
  if (!form.value.username) {
    ElMessage.warning('请输入用户名')
    return
  }
  if (!form.value.password) {
    ElMessage.warning('请输入密码')
    return
  }
  submitting.value = true
  try {
    const res = await createUser({ ...form.value })
    if (res.code !== 200) {
      ElMessage.error(res.message)
      return
    }
    ElMessage.success('新增成功')
    dialogVisible.value = false
    fetch()
  } catch {
    // silent
  } finally {
    submitting.value = false
  }
}

async function handleDelete(row: User) {
  try {
    await ElMessageBox.confirm(`确认删除用户「${row.username}」？`, '提示', {
      type: 'warning',
      confirmButtonText: '确认删除',
      cancelButtonText: '取消',
    })
  } catch {
    return
  }
  try {
    const res = await deleteUser(row.userId)
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
.users-page {
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
</style>
