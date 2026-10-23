<template>
  <div class="user-layout">
    <!-- 吸顶顶栏 -->
    <header class="header">
      <div class="logo" @click="$router.push('/home')">
        <span class="logo-icon">🏞</span>
        <span class="logo-text">景区实时导览</span>
      </div>
      <div class="header-right">
        <template v-if="authStore.token">
          <el-tag :type="authStore.isAdmin ? 'danger' : 'success'" size="small">
            {{ roleText }}
          </el-tag>
          <el-button size="small" text class="logout" @click="handleLogout">退出</el-button>
        </template>
        <el-button v-else type="primary" size="small" @click="$router.push('/login')">登录</el-button>
      </div>
    </header>

    <!-- 单列卡片流内容区（PC 居中 640px） -->
    <main class="main">
      <div class="container">
        <router-view />
      </div>
    </main>

    <!-- 底部五标签栏 -->
    <nav class="tabbar">
      <router-link
        v-for="tab in tabs"
        :key="tab.path"
        :to="tab.path"
        class="tab"
        active-class="active"
      >
        <span class="tab-icon">{{ tab.icon }}</span>
        <span class="tab-label">{{ tab.label }}</span>
      </router-link>
    </nav>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import { useRouter } from 'vue-router'
import { useAuthStore } from '@/stores/auth'

const router = useRouter()
const authStore = useAuthStore()

const tabs = [
  { path: '/home', icon: '🏠', label: '首页' },
  { path: '/spots', icon: '⛰', label: '景点' },
  { path: '/routes', icon: '🧭', label: '路线' },
  { path: '/parking', icon: '🅿', label: '停车' },
  { path: '/shuttles', icon: '🚌', label: '交通' },
]

const roleText = computed(() => {
  if (authStore.role === 'admin') return '管理员'
  if (authStore.role === 'operator') return '运营'
  return '游客'
})

function handleLogout() {
  authStore.logout()
  router.push('/login')
}
</script>

<style scoped>
.user-layout {
  min-height: 100vh;
  background: #f5f7fa;
  display: flex;
  flex-direction: column;
}

.header {
  position: sticky;
  top: 0;
  z-index: 10;
  height: 52px;
  background: #001529;
  color: #fff;
  display: flex;
  align-items: center;
  padding: 0 16px;
}

.logo {
  display: flex;
  align-items: center;
  gap: 8px;
  cursor: pointer;
  font-weight: 600;
  font-size: 15px;
  flex: 1;
}

.logo-icon {
  font-size: 18px;
}

.header-right {
  display: flex;
  align-items: center;
  gap: 8px;
}

.logout {
  color: rgba(255, 255, 255, 0.75);
}

.main {
  flex: 1;
  padding: 12px 12px 76px;
}

.container {
  max-width: 640px;
  margin: 0 auto;
}

.tabbar {
  position: fixed;
  bottom: 0;
  left: 0;
  right: 0;
  z-index: 10;
  height: 56px;
  background: #fff;
  border-top: 1px solid #e4e7ed;
  display: flex;
  justify-content: center;
  padding-bottom: env(safe-area-inset-bottom);
}

.tab {
  flex: 1;
  max-width: 128px;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 2px;
  text-decoration: none;
  color: #909399;
  transition: color 0.2s;
}

.tab-icon {
  font-size: 18px;
  line-height: 1;
}

.tab-label {
  font-size: 11px;
}

.tab.active {
  color: #409eff;
}
</style>
