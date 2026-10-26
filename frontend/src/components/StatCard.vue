<template>
  <el-card class="stat-card" shadow="hover">
    <div class="stat-head">
      <div class="stat-title">{{ title }}</div>
      <div v-if="icon" class="stat-icon">{{ icon }}</div>
    </div>
    <div class="stat-value">
      {{ value }}
      <span v-if="suffix" class="stat-suffix">{{ suffix }}</span>
    </div>
    <div v-if="$slots.default" class="stat-extra">
      <slot />
    </div>
  </el-card>
</template>

<script setup lang="ts">
defineProps<{
  title: string
  value: string | number
  suffix?: string
  icon?: string
}>()
</script>

<style scoped>
.stat-card {
  overflow: hidden;
}

.stat-card :deep(.el-card__body) {
  position: relative;
  padding: 16px 18px 16px 22px;
}

/* 左侧青碧强调条 */
.stat-card :deep(.el-card__body)::before {
  content: '';
  position: absolute;
  left: 0;
  top: 14px;
  bottom: 14px;
  width: 4px;
  border-radius: 0 4px 4px 0;
  background: linear-gradient(180deg, #14b8a6, #0d9488);
  box-shadow: 2px 0 8px rgba(13, 148, 136, 0.3);
}

/* 右上装饰圆环（青碧，极淡） */
.stat-card :deep(.el-card__body)::after {
  content: '';
  position: absolute;
  right: -34px;
  top: -34px;
  width: 96px;
  height: 96px;
  border-radius: 50%;
  border: 16px solid rgba(20, 184, 166, 0.07);
  pointer-events: none;
}

.stat-title {
  font-size: 13px;
  color: var(--sf-text-secondary);
  letter-spacing: 0.5px;
}

.stat-head {
  display: flex;
  align-items: center;
  justify-content: space-between;
  margin-bottom: 8px;
}

.stat-head .stat-title {
  margin-bottom: 0;
}

/* 图标徽章：浅青渐变底 + 柔光 */
.stat-icon {
  width: 34px;
  height: 34px;
  display: flex;
  align-items: center;
  justify-content: center;
  color: #0f766e;
  border-radius: 10px;
  background: linear-gradient(135deg, #e0f5f2, #c9ece8);
  border: 1px solid #bfe6e1;
  box-shadow: 0 3px 8px rgba(13, 148, 136, 0.14);
}

.stat-value {
  font-size: 30px;
  font-weight: 700;
  line-height: 1.2;
  font-variant-numeric: tabular-nums;
  background: linear-gradient(135deg, #123f3c 0%, #0a756c 100%);
  -webkit-background-clip: text;
  background-clip: text;
  color: transparent;
}

.stat-suffix {
  font-size: 14px;
  color: var(--sf-text-secondary);
  font-weight: 400;
  margin-left: 4px;
  /* 后缀保持实色，不随渐变 */
  -webkit-text-fill-color: var(--sf-text-secondary);
}

.stat-extra {
  margin-top: 8px;
}
</style>
