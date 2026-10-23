<template>
  <el-tag :color="tagColor" :style="{ color: '#fff', borderColor: tagColor }" size="small">
    {{ label }}
  </el-tag>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import type { CongestionLevel } from '@/api'
import { getLevelColor, getLevelLabel } from '@/utils/level'

const props = defineProps<{ level: CongestionLevel }>()

const tagColor = computed(() => getLevelColor(props.level))
const label = computed(() => getLevelLabel(props.level))
</script>
