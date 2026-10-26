<template>
  <div class="admin-layout">
    <aside class="sidebar" :class="{ collapsed }">
      <div class="sidebar-logo" @click="$router.push('/admin/overview')">
        <span class="logo-badge">🏞</span>
        <span v-if="!collapsed" class="logo-text">ScenicFlow 管理平台</span>
      </div>

      <el-menu
        :default-active="activeMenu"
        :collapse="collapsed"
        background-color="transparent"
        text-color="rgba(255,255,255,0.68)"
        active-text-color="#ffffff"
        router
        class="sidebar-menu"
      >
        <template v-for="item in visibleMenus" :key="item.title">
          <el-sub-menu v-if="item.children" :index="item.title">
            <template #title>
              <el-icon><component :is="item.icon" /></el-icon>
              <span>{{ item.title }}</span>
            </template>
            <el-menu-item v-for="child in item.children" :key="child.path" :index="child.path!">
              <el-icon><component :is="child.icon" /></el-icon>
              <span>{{ child.title }}</span>
            </el-menu-item>
          </el-sub-menu>
          <el-menu-item v-else :index="item.path!">
            <el-icon><component :is="item.icon" /></el-icon>
            <span>{{ item.title }}</span>
          </el-menu-item>
        </template>
      </el-menu>
    </aside>

    <div class="admin-main">
      <header class="admin-header">
        <el-button text class="collapse-btn" @click="collapsed = !collapsed">
          <el-icon><component :is="collapsed ? Expand : Fold" /></el-icon>
        </el-button>
        <div class="page-title">{{ pageTitle }}</div>

        <div class="header-right">
          <el-button size="small" text @click="$router.push('/home')">用户端预览</el-button>
          <el-tag :type="authStore.role === 'admin' ? 'danger' : 'success'" size="small">
            {{ roleText }}
          </el-tag>
          <span class="username">{{ authStore.username }}</span>
          <el-button size="small" @click="handleLogout">退出</el-button>
        </div>
      </header>

      <main class="admin-content">
        <router-view />
      </main>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import { ElMessage } from 'element-plus'
import { useAuthStore } from '@/stores/auth'
import { ROLE_LABELS } from '@/utils/status'
import {
  Fold,
  Expand,
  Odometer,
  TrendCharts,
  Warning,
  Guide,
  Bell,
  PieChart,
  Location,
  Van,
  EditPen,
  VideoPlay,
  SetUp,
  User,
  Setting,
  Monitor,
} from '@element-plus/icons-vue'

const route = useRoute()
const router = useRouter()
const authStore = useAuthStore()

const collapsed = ref(false)

interface MenuItem {
  title: string
  icon: unknown
  roles: string[]
  path?: string
  children?: MenuItem[]
}

const ALL: string[] = ['admin', 'operator']

const menus: MenuItem[] = [
  { title: '景区概览', path: '/admin/overview', icon: Odometer, roles: ALL },
  { title: '景点流量看板', path: '/admin/flow', icon: TrendCharts, roles: ALL },
  { title: '拥堵预警', path: '/admin/alerts', icon: Warning, roles: ALL },
  { title: '调度建议', path: '/admin/scheduling', icon: Guide, roles: ALL },
  { title: '事件中心', path: '/admin/events', icon: Bell, roles: ALL },
  { title: '数据统计', path: '/admin/statistics', icon: PieChart, roles: ALL },
  { title: '景点管理', path: '/admin/spot-manage', icon: Location, roles: ALL },
  {
    title: '资源管理',
    icon: Van,
    roles: ALL,
    children: [
      { title: '停车场管理', path: '/admin/resources/parking', icon: Location, roles: ALL },
      { title: '摆渡车管理', path: '/admin/resources/shuttles', icon: Van, roles: ALL },
      { title: '摆渡车路线', path: '/admin/resources/routes', icon: Guide, roles: ALL },
    ],
  },
  { title: '数据录入', path: '/admin/entry', icon: EditPen, roles: ['admin'] },
  { title: '模拟配置', path: '/admin/simulation', icon: VideoPlay, roles: ['admin'] },
  { title: '规则与阈值配置', path: '/admin/rules', icon: SetUp, roles: ['admin'] },
  { title: '用户管理', path: '/admin/users', icon: User, roles: ['admin'] },
  { title: '系统设置', path: '/admin/settings', icon: Setting, roles: ['admin'] },
  { title: '系统监控', path: '/admin/monitor', icon: Monitor, roles: ['admin'] },
]

/** 按当前角色过滤菜单（后端接口黑白名单兜底） */
const visibleMenus = computed<MenuItem[]>(() =>
  menus
    .filter((m) => m.roles.includes(authStore.role))
    .map((m) =>
      m.children
        ? {
            ...m,
            children: m.children.filter((c) => c.roles.includes(authStore.role)),
          }
        : m,
    ),
)

const activeMenu = computed(() => route.path)

const pageTitle = computed(() => (route.meta.title as string) || '管理平台')

const roleText = computed(() => ROLE_LABELS[authStore.role] || authStore.role)

function handleLogout() {
  authStore.logout()
  ElMessage.success('已退出登录')
  router.push('/login')
}
</script>

<style scoped>
.admin-layout {
  display: flex;
  min-height: 100vh;
}

/* ---------- 侧边栏：山黛深青渐变 ---------- */
.sidebar {
  width: 208px;
  flex-shrink: 0;
  transition: width 0.25s;
  overflow: hidden;
  background:
    radial-gradient(320px 240px at 0% 0%, rgba(45, 212, 191, 0.22) 0%, transparent 65%),
    linear-gradient(180deg, #0d3d39 0%, #092e2b 55%, #062421 100%);
  box-shadow: 4px 0 18px rgba(6, 36, 33, 0.18);
  position: relative;
  z-index: 11;
}

.sidebar.collapsed {
  width: 64px;
}

.sidebar-logo {
  height: 56px;
  display: flex;
  align-items: center;
  gap: 10px;
  padding: 0 14px;
  color: #fff;
  font-weight: 600;
  font-size: 15px;
  letter-spacing: 0.5px;
  cursor: pointer;
  white-space: nowrap;
  border-bottom: 1px solid rgba(255, 255, 255, 0.09);
}

.logo-badge {
  width: 32px;
  height: 32px;
  flex-shrink: 0;
  display: flex;
  align-items: center;
  justify-content: center;
  border-radius: 9px;
  background: linear-gradient(135deg, #2dd4bf, #0d9488);
  box-shadow: 0 4px 12px rgba(45, 212, 191, 0.45);
}

.logo-badge svg {
  width: 18px;
  height: 18px;
}

.sidebar-menu {
  border-right: none;
  padding: 8px;
}

.sidebar-menu:not(.el-menu--collapse) {
  width: 100%;
}

/* 菜单项：圆角胶囊 + 悬浮态 */
.sidebar-menu :deep(.el-menu-item),
.sidebar-menu :deep(.el-sub-menu__title) {
  border-radius: 9px;
  margin: 2px 0;
  transition:
    background 0.22s ease,
    color 0.22s ease,
    transform 0.22s ease;
}

.sidebar-menu :deep(.el-menu-item:hover),
.sidebar-menu :deep(.el-sub-menu__title:hover) {
  background: rgba(255, 255, 255, 0.08);
  color: #fff;
}

/* 选中项：青碧渐变胶囊 + 柔光 */
.sidebar-menu :deep(.el-menu-item.is-active) {
  background: linear-gradient(135deg, #14b8a6 0%, #0d9488 100%);
  color: #fff;
  font-weight: 600;
  box-shadow:
    0 4px 14px rgba(13, 148, 136, 0.45),
    inset 0 1px 0 rgba(255, 255, 255, 0.22);
}

/* 子菜单弹层底色与侧栏协调 */
.sidebar-menu :deep(.el-menu) {
  background: transparent;
}

.admin-main {
  flex: 1;
  min-width: 0;
  display: flex;
  flex-direction: column;
}

/* ---------- 顶栏：毛玻璃 ---------- */
.admin-header {
  height: 56px;
  display: flex;
  align-items: center;
  gap: 12px;
  padding: 0 20px;
  background: rgba(255, 255, 255, 0.78);
  backdrop-filter: blur(14px) saturate(1.4);
  -webkit-backdrop-filter: blur(14px) saturate(1.4);
  border-bottom: 1px solid rgba(13, 148, 136, 0.1);
  box-shadow: 0 2px 12px rgba(11, 60, 56, 0.06);
  position: sticky;
  top: 0;
  z-index: 10;
}

.collapse-btn {
  font-size: 18px;
  color: var(--sf-ink-soft);
}

/* 页面标题：强调条 + 深青 */
.page-title {
  position: relative;
  padding-left: 12px;
  font-size: 15px;
  font-weight: 600;
  color: var(--sf-ink);
  letter-spacing: 0.5px;
}

.page-title::before {
  content: '';
  position: absolute;
  left: 0;
  top: 50%;
  transform: translateY(-50%);
  width: 4px;
  height: 15px;
  border-radius: 2px;
  background: linear-gradient(180deg, var(--sf-primary-soft), var(--sf-primary));
  box-shadow: 0 2px 6px rgba(13, 148, 136, 0.35);
}

.header-right {
  margin-left: auto;
  display: flex;
  align-items: center;
  gap: 12px;
}

.username {
  font-size: 13px;
  color: var(--sf-text-secondary);
}

.admin-content {
  flex: 1;
  padding: 18px;
  animation: sf-fade-up 0.4s cubic-bezier(0.22, 1, 0.36, 1) both;
}

@media (max-width: 768px) {
  .sidebar {
    width: 64px;
  }

  .sidebar-menu:not(.el-menu--collapse) {
    width: 100%;
  }

  .username {
    display: none;
  }
}
</style>
