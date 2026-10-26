<template>
  <div class="user-layout">
    <!-- 吸顶顶栏 -->
    <header class="header">
      <div class="logo" @click="$router.push('/home')">
        <span class="logo-badge">🏞</span>
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
        <span class="tab-icon" v-html="tab.icon"></span>
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

// 底部标签栏图标：线性 SVG（避免 emoji 带来的模板感）
const line = 'fill="none" stroke="currentColor" stroke-width="1.7" stroke-linecap="round" stroke-linejoin="round"'

const tabs = [
  {
    path: '/home',
    label: '首页',
    icon: `<svg width="18" height="18" viewBox="0 0 24 24" ${line}><path d="M4 10.5L12 4l8 6.5V20a1 1 0 0 1-1 1h-4v-6H9v6H5a1 1 0 0 1-1-1v-9.5z"/></svg>`,
  },
  {
    path: '/spots',
    label: '景点',
    icon: `<svg width="18" height="18" viewBox="0 0 24 24" ${line}><path d="M3 18l5-8 3.5 5 2.5-3.5L20 18H3z"/><circle cx="17" cy="7" r="2"/></svg>`,
  },
  {
    path: '/routes',
    label: '路线',
    icon: `<svg width="18" height="18" viewBox="0 0 24 24" ${line}><circle cx="12" cy="12" r="8.5"/><path d="M15.5 8.5l-2 5-5 2 2-5 5-2z"/></svg>`,
  },
  {
    path: '/parking',
    label: '停车',
    icon: `<svg width="18" height="18" viewBox="0 0 24 24" ${line}><rect x="4" y="4" width="16" height="16" rx="4"/><path d="M10 16V8h3a2.5 2.5 0 0 1 0 5h-3"/></svg>`,
  },
  {
    path: '/shuttles',
    label: '交通',
    icon: `<svg width="18" height="18" viewBox="0 0 24 24" ${line}><rect x="3" y="5" width="18" height="12" rx="2.5"/><path d="M3 10.5h18"/><circle cx="7.5" cy="19" r="1.6"/><circle cx="16.5" cy="19" r="1.6"/></svg>`,
  },
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
  display: flex;
  flex-direction: column;
}

/* ---------- 顶栏：山黛渐变 + 轻投影 ---------- */
.header {
  position: sticky;
  top: 0;
  z-index: 10;
  height: 54px;
  color: #fff;
  display: flex;
  align-items: center;
  padding: 0 16px;
  background:
    radial-gradient(360px 120px at 8% 0%, rgba(45, 212, 191, 0.35) 0%, transparent 70%),
    linear-gradient(120deg, #0d3d39 0%, #0a5a52 55%, #0d9488 100%);
  box-shadow: 0 4px 16px rgba(9, 46, 43, 0.28);
}

.logo {
  display: flex;
  align-items: center;
  gap: 9px;
  cursor: pointer;
  font-weight: 600;
  font-size: 15px;
  letter-spacing: 0.5px;
  flex: 1;
}

.logo-badge {
  width: 30px;
  height: 30px;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 16px;
  border-radius: 9px;
  background: rgba(255, 255, 255, 0.16);
  border: 1px solid rgba(255, 255, 255, 0.22);
  backdrop-filter: blur(4px);
}

.header-right {
  display: flex;
  align-items: center;
  gap: 8px;
}

.logout {
  color: rgba(255, 255, 255, 0.85);
}

.main {
  flex: 1;
  padding: 12px 12px 84px;
}

.container {
  max-width: 640px;
  margin: 0 auto;
  animation: sf-fade-up 0.4s cubic-bezier(0.22, 1, 0.36, 1) both;
}

/* ---------- 底部标签栏：毛玻璃 + 激活胶囊 ---------- */
.tabbar {
  position: fixed;
  bottom: 0;
  left: 0;
  right: 0;
  z-index: 10;
  height: 58px;
  display: flex;
  justify-content: center;
  padding-bottom: env(safe-area-inset-bottom);
  background: rgba(255, 255, 255, 0.85);
  backdrop-filter: blur(16px) saturate(1.5);
  -webkit-backdrop-filter: blur(16px) saturate(1.5);
  border-top: 1px solid rgba(13, 148, 136, 0.12);
  box-shadow: 0 -4px 18px rgba(11, 60, 56, 0.08);
}

.tab {
  position: relative;
  flex: 1;
  max-width: 128px;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 2px;
  text-decoration: none;
  color: #8aa09b;
  transition:
    color 0.22s ease,
    transform 0.22s ease;
}

.tab:active {
  transform: scale(0.94);
}

.tab-icon {
  display: flex;
  align-items: center;
  justify-content: center;
  height: 18px;
  transition: transform 0.25s cubic-bezier(0.22, 1, 0.36, 1);
}

.tab-label {
  font-size: 11px;
}

/* 激活态：青碧色 + 顶部小指示条 + 图标上浮 */
.tab.active {
  color: var(--sf-primary-strong);
  font-weight: 600;
}

.tab.active::before {
  content: '';
  position: absolute;
  top: 0;
  width: 22px;
  height: 3px;
  border-radius: 0 0 3px 3px;
  background: linear-gradient(90deg, var(--sf-primary-soft), var(--sf-primary));
  box-shadow: 0 2px 6px rgba(13, 148, 136, 0.4);
}

.tab.active .tab-icon {
  transform: translateY(-1px) scale(1.12);
}
</style>
