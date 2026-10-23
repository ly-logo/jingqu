<template>
  <div class="login-page">
    <el-card class="login-card" shadow="always">
      <div class="login-header">
        <div class="login-logo">🏞</div>
        <h2 class="login-title">ScenicFlow 景区流量监测与调度系统</h2>
      </div>

      <div class="mode-tabs">
        <div
          class="mode-tab"
          :class="{ active: mode === 'login' }"
          @click="switchMode('login')"
        >
          登录
        </div>
        <div
          class="mode-tab"
          :class="{ active: mode === 'register' }"
          @click="switchMode('register')"
        >
          注册
        </div>
      </div>

      <el-form :model="form" label-position="top" @submit.prevent="handleSubmit">
        <el-form-item label="用户名">
          <el-input
            v-model="form.username"
            :placeholder="mode === 'register' ? '3~20 位字母、数字或下划线' : '请输入用户名'"
            clearable
          />
        </el-form-item>
        <el-form-item label="密码">
          <el-input
            v-model="form.password"
            type="password"
            :placeholder="mode === 'register' ? '6~32 位' : '请输入密码'"
            show-password
          />
        </el-form-item>
        <el-form-item v-if="mode === 'register'" label="确认密码">
          <el-input
            v-model="form.confirmPassword"
            type="password"
            placeholder="请再次输入密码"
            show-password
          />
        </el-form-item>
        <el-form-item>
          <el-button type="primary" :loading="loading" style="width: 100%" @click="handleSubmit">
            {{ mode === 'login' ? '登录' : '注册并登录' }}
          </el-button>
        </el-form-item>
      </el-form>

      <div class="login-footer">
        <template v-if="mode === 'login'">
          还没有账号？<el-link type="primary" :underline="false" @click="switchMode('register')">
            立即注册
          </el-link>
        </template>
        <template v-else>
          已有账号？<el-link type="primary" :underline="false" @click="switchMode('login')">
            返回登录
          </el-link>
        </template>
      </div>
    </el-card>
  </div>
</template>

<script setup lang="ts">
import { reactive, ref } from 'vue'
import { useRouter } from 'vue-router'
import { ElMessage } from 'element-plus'
import { useAuthStore } from '@/stores/auth'

const router = useRouter()
const authStore = useAuthStore()
const loading = ref(false)
const mode = ref<'login' | 'register'>('login')

const form = reactive({
  username: '',
  password: '',
  confirmPassword: '',
})

function switchMode(next: 'login' | 'register') {
  mode.value = next
  form.password = ''
  form.confirmPassword = ''
}

async function handleSubmit() {
  if (!form.username || !form.password) {
    ElMessage.warning('请输入用户名和密码')
    return
  }
  if (mode.value === 'register') {
    if (!/^[A-Za-z0-9_]{3,20}$/.test(form.username)) {
      ElMessage.warning('用户名需为 3~20 位字母、数字或下划线')
      return
    }
    if (form.password.length < 6 || form.password.length > 32) {
      ElMessage.warning('密码长度需为 6~32 位')
      return
    }
    if (form.password !== form.confirmPassword) {
      ElMessage.warning('两次输入的密码不一致')
      return
    }
  }

  loading.value = true
  try {
    const payload = { username: form.username, password: form.password }
    const data =
      mode.value === 'login'
        ? await authStore.login(payload)
        : await authStore.register(payload)
    ElMessage.success(mode.value === 'login' ? '登录成功' : '注册成功，已自动登录')
    if (data.role === 'admin' || data.role === 'operator') {
      router.push('/admin/overview')
    } else {
      router.push('/home')
    }
  } catch {
    // 接口错误的提示已由 axios 响应拦截器统一弹出，这里不再重复提示
  } finally {
    loading.value = false
  }
}
</script>

<style scoped>
.login-page {
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  background: #f5f7fa;
  padding: 24px;
}

.login-card {
  width: 100%;
  max-width: 420px;
  border-radius: 8px;
}

.login-header {
  text-align: center;
  margin-bottom: 16px;
}

.login-logo {
  font-size: 48px;
  line-height: 1;
  margin-bottom: 8px;
}

.login-title {
  font-size: 18px;
  font-weight: 600;
  color: #303133;
  margin: 0;
}

.mode-tabs {
  display: flex;
  border-bottom: 1px solid #e4e7ed;
  margin-bottom: 20px;
}

.mode-tab {
  flex: 1;
  text-align: center;
  padding: 8px 0;
  font-size: 15px;
  color: #909399;
  cursor: pointer;
  border-bottom: 2px solid transparent;
  transition: all 0.2s;
}

.mode-tab.active {
  color: #409eff;
  border-bottom-color: #409eff;
  font-weight: 600;
}

.login-footer {
  text-align: center;
  font-size: 13px;
  color: #909399;
}
</style>
