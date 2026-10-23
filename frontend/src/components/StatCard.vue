<template>
  <el-card class="stat-card" shadow="hover">
    <div class="stat-title">{{ title }}</div>
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
}>()
</script>

<style scoped>
.stat-card {
  border-radius: 6px;
}

.stat-title {
  font-size: 13px;
  color: #909399;
  margin-bottom: 8px;
}

.stat-value {
  font-size: 28px;
  font-weight: 600;
  color: #303133;
  line-height: 1.2;
}

.stat-suffix {
  font-size: 14px;
  color: #909399;
  font-weight: 400;
  margin-left: 4px;
}

.stat-extra {
  margin-top: 8px;
}
</style>
