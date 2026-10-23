<template>
  <el-button size="small" type="primary" plain class="nav-button" @click="openNav">
    导航
  </el-button>
</template>

<script setup lang="ts">
// 一键导航：唤起高德地图 URI（移动端调起 App，PC 端打开网页版路线规划）
const props = defineProps<{
  name: string
  latitude: number
  longitude: number
}>()

function openNav() {
  const lat = props.latitude
  const lng = props.longitude
  const url =
    `https://uri.amap.com/navigation?to=${lng},${lat},${encodeURIComponent(props.name)}` +
    `&mode=walk&policy=1&src=scenicflow&coordinate=gaode&callnative=1`
  window.open(url, '_blank')
}
</script>

<style scoped>
.nav-button {
  flex-shrink: 0;
}
</style>
