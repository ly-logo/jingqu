<template>
  <div class="login-page">
    <!-- 背景装饰：光斑 + 三层山峦剪影 + 山脚公路 -->
    <div class="bg-decor" aria-hidden="true">
      <span class="orb orb-1"></span>
      <span class="orb orb-2"></span>
      <span class="orb orb-3"></span>

      <svg class="mountains" viewBox="0 0 1440 360" preserveAspectRatio="none">
        <path
          class="mt-far"
          d="M0,196 L110,150 L230,198 L360,120 L500,186 L640,112 L780,178 L920,132 L1060,192 L1200,146 L1320,190 L1440,160 L1440,360 L0,360 Z"
        />
        <path
          class="mt-mid"
          d="M0,242 L150,198 L300,248 L450,180 L610,240 L770,176 L930,238 L1090,196 L1250,246 L1440,206 L1440,360 L0,360 Z"
        />
        <path
          class="mt-near"
          d="M0,296 L180,258 L340,300 L520,250 L700,296 L880,246 L1060,298 L1240,262 L1440,300 L1440,360 L0,360 Z"
        />
      </svg>

      <!-- 左右两侧山野装饰：云雾、飞鸟、竹林、松树、落叶 -->
      <div class="side-decor">
        <div class="mist mist-a"></div>
        <div class="mist mist-b"></div>

        <svg class="bird bird-1" viewBox="0 0 24 12">
          <path
            class="wing"
            d="M1 10c4-6 7-6 11 0 4-6 7-6 11 0"
            fill="none"
            stroke="rgba(255,255,255,0.5)"
            stroke-width="1.6"
            stroke-linecap="round"
          />
        </svg>
        <svg class="bird bird-2" viewBox="0 0 24 12">
          <path
            class="wing"
            d="M1 10c4-6 7-6 11 0 4-6 7-6 11 0"
            fill="none"
            stroke="rgba(255,255,255,0.38)"
            stroke-width="1.6"
            stroke-linecap="round"
          />
        </svg>
        <svg class="bird bird-3" viewBox="0 0 24 12">
          <path
            class="wing"
            d="M1 10c4-6 7-6 11 0 4-6 7-6 11 0"
            fill="none"
            stroke="rgba(255,255,255,0.3)"
            stroke-width="1.6"
            stroke-linecap="round"
          />
        </svg>

        <svg class="cable" viewBox="0 0 200 260" fill="none">
          <!-- 山脚支架 -->
          <path d="M22 236v-38" stroke="rgba(200,245,238,0.32)" stroke-width="5" stroke-linecap="round" />
          <path
            d="M10 214l12-16 12 16"
            stroke="rgba(200,245,238,0.26)"
            stroke-width="3"
            stroke-linecap="round"
            stroke-linejoin="round"
          />
          <!-- 双缆绳 -->
          <path d="M22 200L196 34" stroke="rgba(200,245,238,0.3)" stroke-width="2" />
          <path d="M22 212L196 46" stroke="rgba(200,245,238,0.14)" stroke-width="2" />
          <!-- 缆车厢：沿缆绳上下滑行 -->
          <g transform="translate(39 183)">
            <g class="cable-cabin">
              <g class="cabin-swing">
                <path d="M0 0v16" stroke="rgba(235,252,249,0.85)" stroke-width="2.4" stroke-linecap="round" />
                <rect x="-27" y="15" width="54" height="42" rx="9" fill="#f3faf8" />
                <rect x="-27" y="44" width="54" height="13" rx="6" fill="#dcece9" />
                <rect x="-21" y="22" width="17" height="13" rx="3" fill="#083330" opacity="0.82" />
                <rect x="-1" y="22" width="17" height="13" rx="3" fill="#083330" opacity="0.82" />
                <rect x="-27" y="40" width="54" height="5" fill="#0d9488" />
              </g>
            </g>
          </g>
        </svg>

        <svg class="waterwheel" viewBox="0 0 180 200">
          <!-- 支架与底座 -->
          <path
            d="M52 192l38-104M128 192L90 88"
            stroke="rgba(200,245,238,0.3)"
            stroke-width="5"
            stroke-linecap="round"
          />
          <rect x="28" y="188" width="124" height="8" rx="4" fill="rgba(200,245,238,0.16)" />
          <!-- 转轮 -->
          <g class="wheel-rotate">
            <circle cx="90" cy="88" r="62" fill="none" stroke="rgba(210,245,240,0.45)" stroke-width="4" />
            <circle cx="90" cy="88" r="52" fill="none" stroke="rgba(210,245,240,0.22)" stroke-width="2" />
            <rect
              v-for="i in 8"
              :key="i"
              x="86"
              y="16"
              width="8"
              height="18"
              rx="2"
              fill="rgba(210,245,240,0.22)"
              :transform="`rotate(${i * 45} 90 88)`"
            />
            <g stroke="rgba(210,245,240,0.32)" stroke-width="3" stroke-linecap="round">
              <path d="M90 26v124" />
              <path d="M28 88h124" />
              <path d="M46 44l88 88" />
              <path d="M134 44L46 132" />
            </g>
            <circle cx="90" cy="88" r="9" fill="#e8b25a" opacity="0.5" />
          </g>
          <!-- 水花 -->
          <g class="splash" fill="rgba(210,245,240,0.5)">
            <circle cx="58" cy="182" r="3" />
            <circle cx="70" cy="178" r="2.4" />
            <circle cx="46" cy="178" r="2" />
          </g>
        </svg>

        <span class="leaf leaf-1"></span>
        <span class="leaf leaf-2"></span>
        <span class="leaf leaf-3"></span>
      </div>

      <!-- 山脚公路：行驶中的摆渡大巴 -->
      <div class="road-scene">
        <div class="road-line"></div>
        <div class="bus-driver">
          <svg class="bus-art" viewBox="0 0 200 96">
            <!-- 尾气 -->
            <g class="exhaust" fill="#e9f7f5">
              <circle cx="8" cy="56" r="4" />
              <circle cx="6" cy="58" r="4" />
              <circle cx="4" cy="60" r="4" />
            </g>

            <!-- 车顶空调 -->
            <rect x="66" y="4" width="44" height="7" rx="3.5" fill="#d3e8e4" />

            <!-- 车身 -->
            <rect x="8" y="11" width="184" height="59" rx="13" fill="#f3faf8" />
            <rect x="8" y="52" width="184" height="18" rx="9" fill="#dcece9" />
            <rect x="11" y="43" width="178" height="6" fill="#0d9488" />

            <!-- 车窗 -->
            <g fill="#083330" opacity="0.85">
              <rect x="20" y="21" width="26" height="16" rx="3" />
              <rect x="51" y="21" width="26" height="16" rx="3" />
              <rect x="82" y="21" width="26" height="16" rx="3" />
              <rect x="113" y="21" width="26" height="16" rx="3" />
              <rect x="145" y="21" width="32" height="16" rx="3" />
            </g>

            <!-- 前照灯 / 尾灯 -->
            <rect x="180" y="33" width="8" height="9" rx="4" fill="#ffd98a" />
            <rect x="10" y="34" width="6" height="8" rx="3" fill="#f2836f" />

            <!-- 车轮 -->
            <g>
              <circle cx="45" cy="73" r="13" fill="#0b2b29" />
              <g class="wheel-spin">
                <circle cx="45" cy="73" r="6" fill="#9dc4bd" />
                <path
                  d="M45 68.5v9M40.5 73h9"
                  stroke="#0b2b29"
                  stroke-width="1.4"
                  stroke-linecap="round"
                />
              </g>
              <circle cx="152" cy="73" r="13" fill="#0b2b29" />
              <g class="wheel-spin">
                <circle cx="152" cy="73" r="6" fill="#9dc4bd" />
                <path
                  d="M152 68.5v9M147.5 73h9"
                  stroke="#0b2b29"
                  stroke-width="1.4"
                  stroke-linecap="round"
                />
              </g>
            </g>
          </svg>
        </div>
      </div>
    </div>

    <div class="login-card sf-fade-up">
      <!-- 左侧品牌区（宽屏显示） -->
      <div class="brand-panel">
        <div class="brand-logo">
          <svg viewBox="0 0 24 24" fill="none">
            <path d="M2 18.5l6-10 4.5 6 3-4 6.5 8H2z" fill="#fff" />
            <circle cx="17" cy="6.2" r="2" fill="#e8b25a" />
          </svg>
        </div>
        <h1 class="brand-name">ScenicFlow</h1>
        <p class="brand-slogan">景区流量监测与智能调度</p>
        <ul class="brand-points">
          <li>
            <span class="point-icon">
              <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.8">
                <path d="M3 20V11M9 20V5M15 20v-6M21 20H2" stroke-linecap="round" />
              </svg>
            </span>
            实时客流热力，一屏总览全园
          </li>
          <li>
            <span class="point-icon">
              <svg
                viewBox="0 0 24 24"
                fill="none"
                stroke="currentColor"
                stroke-width="1.8"
                stroke-linejoin="round"
              >
                <path d="M12 3.6L21.4 20H2.6L12 3.6z" />
                <path d="M12 10v4" stroke-linecap="round" />
                <circle cx="12" cy="16.8" r="0.9" fill="currentColor" stroke="none" />
              </svg>
            </span>
            拥堵预警去抖，调度精准触达
          </li>
          <li>
            <span class="point-icon">
              <svg
                viewBox="0 0 24 24"
                fill="none"
                stroke="currentColor"
                stroke-width="1.8"
                stroke-linejoin="round"
              >
                <rect x="3" y="5" width="18" height="12" rx="2.5" />
                <path d="M3 10h18" />
                <circle cx="7.5" cy="19" r="1.8" />
                <circle cx="16.5" cy="19" r="1.8" />
              </svg>
            </span>
            停车与摆渡联动，错峰游园引导
          </li>
        </ul>
        <div class="brand-footer">武陵源景区 · 智慧文旅示范项目</div>
      </div>

      <!-- 右侧表单区 -->
      <div class="form-panel">
        <div class="login-header">
          <div class="login-logo">
            <svg viewBox="0 0 24 24" fill="none">
              <path d="M2 18.5l6-10 4.5 6 3-4 6.5 8H2z" fill="#fff" />
              <circle cx="17" cy="6.2" r="2" fill="#e8b25a" />
            </svg>
          </div>
          <h2 class="login-title">
            {{ mode === 'login' ? '欢迎回来' : '创建账号' }}
          </h2>
          <p class="login-subtitle">
            {{ mode === 'login' ? '登录景区流量监测与调度系统' : '注册后以游客身份体验实时导览' }}
          </p>
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
              size="large"
            />
          </el-form-item>
          <el-form-item label="密码">
            <el-input
              v-model="form.password"
              type="password"
              :placeholder="mode === 'register' ? '6~32 位' : '请输入密码'"
              show-password
              size="large"
            />
          </el-form-item>
          <el-form-item v-if="mode === 'register'" label="确认密码">
            <el-input
              v-model="form.confirmPassword"
              type="password"
              placeholder="请再次输入密码"
              show-password
              size="large"
            />
          </el-form-item>
          <el-form-item>
            <el-button
              type="primary"
              size="large"
              :loading="loading"
              class="submit-btn"
              @click="handleSubmit"
            >
              {{ mode === 'login' ? '登 录' : '注册并登录' }}
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

        <!-- 演示账号快捷填充 -->
        <div v-if="mode === 'login'" class="demo-accounts">
          <span class="demo-label">演示账号（密码 123456）</span>
          <div class="demo-chips">
            <span class="chip" @click="fillDemo('admin')">管理员</span>
            <span class="chip" @click="fillDemo('operator')">运营</span>
            <span class="chip" @click="fillDemo('viewer')">游客</span>
          </div>
        </div>
      </div>
    </div>
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

/** 快捷填充演示账号 */
function fillDemo(username: string) {
  form.username = username
  form.password = '123456'
}

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
  position: relative;
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 32px 24px;
  overflow: hidden;
  background: linear-gradient(155deg, #062826 0%, #0a4a45 42%, #0d9488 100%);
}

/* ---------- 背景装饰 ---------- */
.bg-decor {
  position: absolute;
  inset: 0;
  pointer-events: none;
}

.orb {
  position: absolute;
  border-radius: 50%;
  filter: blur(70px);
  opacity: 0.55;
}

.orb-1 {
  width: 420px;
  height: 420px;
  left: -120px;
  top: -100px;
  background: radial-gradient(circle, #2dd4bf 0%, transparent 70%);
  animation: sf-float 9s ease-in-out infinite;
}

.orb-2 {
  width: 360px;
  height: 360px;
  right: -80px;
  bottom: 40px;
  background: radial-gradient(circle, rgba(232, 178, 90, 0.7) 0%, transparent 70%);
  animation: sf-float 11s ease-in-out infinite reverse;
}

.orb-3 {
  width: 300px;
  height: 300px;
  left: 55%;
  top: 12%;
  background: radial-gradient(circle, rgba(45, 212, 191, 0.45) 0%, transparent 70%);
  animation: sf-float 13s ease-in-out infinite;
}

.mountains {
  position: absolute;
  left: 0;
  right: 0;
  bottom: 0;
  width: 100%;
  height: 320px;
}

/* 远 / 中 / 近三层山峦：越近越沉，制造纵深 */
.mt-far {
  fill: rgba(255, 255, 255, 0.06);
}

.mt-mid {
  fill: rgba(4, 46, 43, 0.26);
}

.mt-near {
  fill: rgba(3, 34, 32, 0.5);
}

/* ---------- 左右两侧山野装饰 ---------- */
.side-decor {
  position: absolute;
  inset: 0;
}

/* 云雾带：缓慢左右流动 */
.mist {
  position: absolute;
  left: -15%;
  width: 130%;
  height: 140px;
  filter: blur(28px);
  opacity: 0.55;
  background: linear-gradient(
    90deg,
    transparent 0%,
    rgba(255, 255, 255, 0.1) 45%,
    rgba(255, 255, 255, 0.04) 60%,
    transparent 100%
  );
}

.mist-a {
  top: 22%;
  animation: mist-drift 26s ease-in-out infinite alternate;
}

.mist-b {
  top: 58%;
  height: 110px;
  animation: mist-drift 34s ease-in-out infinite alternate-reverse;
}

@keyframes mist-drift {
  from {
    transform: translateX(-5%);
  }
  to {
    transform: translateX(5%);
  }
}

/* 飞鸟：横向掠过 + 翅膀扇动 */
.bird {
  position: absolute;
  left: 0;
  width: 26px;
  height: 13px;
}

.bird-1 {
  top: 11%;
  animation: bird-fly 42s linear infinite;
}

.bird-2 {
  top: 16%;
  width: 20px;
  animation: bird-fly 54s linear infinite 7s;
}

.bird-3 {
  top: 8%;
  width: 15px;
  animation: bird-fly 66s linear infinite 17s;
}

@keyframes bird-fly {
  0% {
    transform: translate(-70px, 0);
  }
  50% {
    transform: translate(50vw, -22px);
  }
  100% {
    transform: translate(calc(100vw + 70px), 6px);
  }
}

.wing {
  transform-box: fill-box;
  transform-origin: center;
  animation: wing-flap 1.1s ease-in-out infinite alternate;
}

@keyframes wing-flap {
  from {
    transform: scaleY(1);
  }
  to {
    transform: scaleY(0.45);
  }
}

/* 左侧缆车：车厢沿缆绳往返滑行 + 吊厢轻微摆动 */
.cable {
  position: absolute;
  left: 6px;
  bottom: 104px;
  width: 200px;
  height: 260px;
}

.cable-cabin {
  animation: cable-ride 11s cubic-bezier(0.45, 0.05, 0.55, 0.95) infinite alternate;
}

@keyframes cable-ride {
  from {
    transform: translate(0, 0);
  }
  to {
    transform: translate(113px, -108px);
  }
}

.cabin-swing {
  transform-box: fill-box;
  transform-origin: 50% 0%;
  animation: cabin-swing 4.2s ease-in-out infinite;
}

@keyframes cabin-swing {
  0%,
  100% {
    transform: rotate(-3deg);
  }
  50% {
    transform: rotate(3deg);
  }
}

/* 右侧水车：转轮持续旋转 + 水花飞溅 */
.waterwheel {
  position: absolute;
  left: calc(50% + 490px);
  bottom: 86px;
  width: 180px;
  height: 200px;
}

.wheel-rotate {
  transform-box: fill-box;
  transform-origin: center;
  animation: wheel-rotate 14s linear infinite;
}

@keyframes wheel-rotate {
  to {
    transform: rotate(360deg);
  }
}

.splash circle {
  transform-box: fill-box;
  transform-origin: center;
  animation: splash-up 2.2s ease-out infinite;
  opacity: 0;
}

.splash circle:nth-child(2) {
  animation-delay: 0.7s;
}

.splash circle:nth-child(3) {
  animation-delay: 1.4s;
}

@keyframes splash-up {
  0% {
    transform: translate(0, 0) scale(0.5);
    opacity: 0.55;
  }
  70% {
    opacity: 0.2;
  }
  100% {
    transform: translate(-8px, -26px) scale(1.4);
    opacity: 0;
  }
}

/* 落叶：自右上飘落并翻转 */
.leaf {
  position: absolute;
  top: 0;
  width: 9px;
  height: 14px;
  background: rgba(232, 178, 90, 0.42);
  border-radius: 0 100% 0 100%;
}

.leaf-1 {
  right: 7%;
  animation: leaf-fall 15s linear infinite;
}

.leaf-2 {
  right: 15%;
  animation: leaf-fall 19s linear infinite 4s;
}

.leaf-3 {
  right: 4%;
  animation: leaf-fall 17s linear infinite 9s;
}

@keyframes leaf-fall {
  0% {
    transform: translate(0, -40px) rotate(0deg);
    opacity: 0;
  }
  12% {
    opacity: 0.55;
  }
  85% {
    opacity: 0.3;
  }
  100% {
    transform: translate(-46px, 100vh) rotate(320deg);
    opacity: 0;
  }
}

/* ---------- 山脚公路 ---------- */
.road-scene {
  position: absolute;
  left: 0;
  right: 0;
  bottom: 0;
  height: 112px;
  overflow: hidden;
}

.road-line {
  position: absolute;
  left: 0;
  right: 0;
  bottom: 30px;
  height: 2px;
  background: repeating-linear-gradient(
    90deg,
    rgba(255, 255, 255, 0.32) 0 38px,
    transparent 38px 92px
  );
  animation: road-move 1.4s linear infinite;
}

@keyframes road-move {
  from {
    background-position-x: 0;
  }
  to {
    background-position-x: -92px;
  }
}

/* 大巴自左向右驶过 */
.bus-driver {
  position: absolute;
  bottom: 24px;
  left: 0;
  animation: bus-drive 19s linear infinite;
}

@keyframes bus-drive {
  0% {
    transform: translateX(-230px);
  }
  100% {
    transform: translateX(calc(100vw + 40px));
  }
}

.bus-art {
  display: block;
  width: 156px;
  filter: drop-shadow(0 10px 16px rgba(0, 0, 0, 0.38));
  animation: bus-bob 0.72s ease-in-out infinite;
}

/* 路面颠簸：整车轻微上下浮动 */
@keyframes bus-bob {
  0%,
  100% {
    transform: translateY(0);
  }
  50% {
    transform: translateY(-2.5px);
  }
}

/* 车轮转动 */
.wheel-spin {
  transform-box: fill-box;
  transform-origin: center;
  animation: wheel-spin 0.55s linear infinite;
}

@keyframes wheel-spin {
  to {
    transform: rotate(360deg);
  }
}

/* 尾气：淡出并向上飘散 */
.exhaust circle {
  transform-box: fill-box;
  transform-origin: center;
  animation: exhaust-puff 1.8s ease-out infinite;
  opacity: 0;
}

.exhaust circle:nth-child(2) {
  animation-delay: 0.6s;
}

.exhaust circle:nth-child(3) {
  animation-delay: 1.2s;
}

@keyframes exhaust-puff {
  0% {
    transform: translate(0, 0) scale(0.4);
    opacity: 0.5;
  }
  70% {
    opacity: 0.16;
  }
  100% {
    transform: translate(-30px, -14px) scale(1.6);
    opacity: 0;
  }
}

/* ---------- 双栏卡片 ---------- */
.login-card {
  position: relative;
  z-index: 1;
  width: 100%;
  max-width: 880px;
  display: flex;
  border-radius: 24px;
  overflow: hidden;
  background: rgba(255, 255, 255, 0.96);
  box-shadow:
    0 30px 80px rgba(3, 30, 28, 0.45),
    0 0 0 1px rgba(255, 255, 255, 0.12);
  backdrop-filter: blur(12px);
}

/* ---------- 左侧品牌 ---------- */
.brand-panel {
  flex: 1.05;
  position: relative;
  padding: 48px 40px;
  color: #fff;
  display: flex;
  flex-direction: column;
  justify-content: center;
  background:
    radial-gradient(circle at 80% 0%, rgba(45, 212, 191, 0.35) 0%, transparent 55%),
    linear-gradient(165deg, #0f5f59 0%, #0a3f3b 60%, #072e2b 100%);
  overflow: hidden;
}

.brand-panel::after {
  content: '';
  position: absolute;
  right: -60px;
  bottom: -60px;
  width: 220px;
  height: 220px;
  border-radius: 50%;
  border: 24px solid rgba(232, 178, 90, 0.12);
}

.brand-logo {
  width: 56px;
  height: 56px;
  border-radius: 16px;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 30px;
  background: linear-gradient(135deg, #2dd4bf, #0d9488);
  box-shadow: 0 8px 20px rgba(45, 212, 191, 0.4);
  margin-bottom: 20px;
}

.brand-name {
  font-size: 30px;
  font-weight: 700;
  letter-spacing: 1px;
  margin: 0;
}

.brand-slogan {
  margin: 8px 0 28px;
  font-size: 14px;
  color: rgba(255, 255, 255, 0.72);
  letter-spacing: 1px;
}

.brand-points {
  list-style: none;
  padding: 0;
  margin: 0;
  display: flex;
  flex-direction: column;
  gap: 14px;
}

.brand-points li {
  display: flex;
  align-items: center;
  gap: 10px;
  font-size: 13.5px;
  color: rgba(255, 255, 255, 0.88);
}

.brand-points li span {
  width: 28px;
  height: 28px;
  flex-shrink: 0;
  display: flex;
  align-items: center;
  justify-content: center;
  border-radius: 8px;
  background: rgba(255, 255, 255, 0.1);
}

.brand-points li span svg {
  width: 15px;
  height: 15px;
  color: #7be3d7;
}

.brand-footer {
  margin-top: 36px;
  font-size: 12px;
  color: rgba(255, 255, 255, 0.45);
  letter-spacing: 0.5px;
}

/* ---------- 右侧表单 ---------- */
.form-panel {
  flex: 1;
  padding: 44px 44px 32px;
  min-width: 0;
}

.login-header {
  text-align: center;
  margin-bottom: 20px;
}

.login-logo {
  display: none;
  width: 52px;
  height: 52px;
  margin: 0 auto 12px;
  border-radius: 14px;
  align-items: center;
  justify-content: center;
  background: linear-gradient(135deg, #2dd4bf, #0d9488);
  box-shadow: 0 8px 18px rgba(13, 148, 136, 0.35);
}

.login-logo svg {
  width: 28px;
  height: 28px;
}

.login-title {
  font-size: 22px;
  font-weight: 700;
  color: #123f3c;
  margin: 0;
}

.login-subtitle {
  margin: 8px 0 0;
  font-size: 13px;
  color: #7a8e8a;
}

.mode-tabs {
  display: flex;
  gap: 8px;
  padding: 4px;
  margin-bottom: 22px;
  background: #eef6f5;
  border-radius: 12px;
}

.mode-tab {
  flex: 1;
  text-align: center;
  padding: 9px 0;
  font-size: 14px;
  font-weight: 500;
  color: #6e8581;
  cursor: pointer;
  border-radius: 9px;
  transition: all 0.25s cubic-bezier(0.22, 1, 0.36, 1);
}

.mode-tab.active {
  color: #fff;
  font-weight: 600;
  background: linear-gradient(135deg, #14b8a6, #0d9488);
  box-shadow: 0 4px 12px rgba(13, 148, 136, 0.35);
}

.submit-btn {
  width: 100%;
  font-weight: 600;
}

.login-footer {
  text-align: center;
  font-size: 13px;
  color: #90a09c;
}

/* ---------- 演示账号 ---------- */
.demo-accounts {
  margin-top: 22px;
  padding-top: 18px;
  border-top: 1px dashed #dce8e5;
  text-align: center;
}

.demo-label {
  display: block;
  font-size: 12px;
  color: #90a09c;
  margin-bottom: 10px;
}

.demo-chips {
  display: flex;
  justify-content: center;
  gap: 10px;
}

.chip {
  padding: 5px 16px;
  font-size: 12.5px;
  color: #0a756c;
  background: #e9f7f5;
  border: 1px solid #cbe8e4;
  border-radius: 999px;
  cursor: pointer;
  transition: all 0.2s ease;
  user-select: none;
}

.chip:hover {
  background: linear-gradient(135deg, #14b8a6, #0d9488);
  color: #fff;
  border-color: transparent;
  transform: translateY(-1px);
  box-shadow: 0 4px 10px rgba(13, 148, 136, 0.3);
}

/* ---------- 响应式：窄屏隐藏品牌面板 ---------- */
/* 两侧装饰以登录卡片为基准定位：宽屏紧贴卡片外侧，窄屏自然裁切而不压卡 */
@media (max-width: 1150px) {
  .side-decor {
    display: none;
  }
}

@media (max-width: 760px) {
  .brand-panel {
    display: none;
  }

  .road-scene {
    display: none;
  }

  .form-panel {
    padding: 32px 24px 24px;
  }

  .login-logo {
    display: flex;
  }

  .login-card {
    max-width: 420px;
  }
}
</style>
