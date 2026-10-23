<template>
  <span class="queue-badge" :class="toneClass">⏱ {{ prefix }}约 {{ minutes }} 分钟</span>
</template>

<script setup lang="ts">
import { computed } from 'vue'

// 排队/候车时长徽标：拥挤度高时显示为警示色
const props = withDefaults(
  defineProps<{
    minutes: number
    prefix?: string
    /** tone 决定配色：normal 灰 / warn 橙 / danger 红 */
    tone?: 'auto' | 'normal'
  }>(),
  { prefix: '排队', tone: 'auto' },
)

const toneClass = computed(() => {
  if (props.tone === 'normal') return 'tone-normal'
  if (props.minutes >= 45) return 'tone-danger'
  if (props.minutes >= 20) return 'tone-warn'
  return 'tone-normal'
})
</script>

<style scoped>
.queue-badge {
  display: inline-flex;
  align-items: center;
  gap: 2px;
  font-size: 12px;
  line-height: 1;
  padding: 4px 8px;
  border-radius: 10px;
  white-space: nowrap;
}

.tone-normal {
  background: #f0f2f5;
  color: #606266;
}

.tone-warn {
  background: #fdf6ec;
  color: #e6a23c;
}

.tone-danger {
  background: #fef0f0;
  color: #f56c6c;
}
</style>
