<template>
  <div class="amap-wrap" :style="{ height }">
    <div ref="mapRef" class="amap"></div>
    <div v-if="loadFailed" class="map-fallback">高德地图加载失败，请检查网络后刷新</div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted, watch } from 'vue'
import type { MapMarker } from '@/api'
import { getLevelColor } from '@/utils/level'

// 高德地图通用组件：底图 + 点位标记 + 路线折线
// 地图中心 / 缩放按需求固定在武陵源景区内部
const MAP_CENTER: [number, number] = [110.466, 29.346]
const MAP_ZOOM = 15

const props = withDefaults(
  defineProps<{
    points: MapMarker[]
    routePath?: MapMarker[]
    lineColor?: string
    focusId?: string
    height?: string
  }>(),
  { routePath: () => [], lineColor: '#67C23A', focusId: '', height: '420px' },
)

const emit = defineEmits<{ 'marker-click': [marker: MapMarker] }>()

const mapRef = ref<HTMLDivElement>()
const loadFailed = ref(false)

let map: any = null
let markers: any[] = []
let polyline: any = null
let infoWindow: any = null

// 类型配色：闸机红、停车场蓝、景点绿
function markerColor(m: MapMarker): string {
  if (m.type === 'gate') return '#F56C6C'
  if (m.type === 'parking') return '#409EFF'
  return '#67C23A'
}

function typeText(m: MapMarker): string {
  if (m.type === 'gate') return '闸机'
  if (m.type === 'parking') return '停车场'
  return '景点'
}

// 弹窗 HTML：名称、类型、人数、拥挤度 / 剩余车位
function infoHtml(m: MapMarker): string {
  const rows = [`<div><b>${m.name}</b></div>`, `<div>类型：${typeText(m)}</div>`]
  if (m.visitors !== undefined) rows.push(`<div>当前人数：${m.visitors} 人</div>`)
  if (m.levelText) {
    rows.push(
      `<div>拥挤度：<span style="color:#fff;background:${getLevelColor(m.level || 'NORMAL')};` +
        `padding:1px 6px;border-radius:3px;">${m.levelText}</span></div>`,
    )
  }
  if (m.freeSpaces !== undefined) rows.push(`<div>剩余车位：${m.freeSpaces} 个</div>`)
  return `<div style="font-size:13px;line-height:1.7;color:#303133;min-width:150px;">${rows.join('')}</div>`
}

function renderMarkers() {
  if (!map) return
  markers.forEach((m) => map.remove(m))
  markers = []

  for (const p of props.points) {
    if (!p.longitude || !p.latitude) continue
    const color = markerColor(p)
    const marker = new window.AMap.Marker({
      position: [p.longitude, p.latitude],
      title: p.name,
      anchor: 'center',
      content:
        `<div style="width:14px;height:14px;border-radius:50%;background:${color};` +
        `border:2px solid #fff;box-shadow:0 1px 4px rgba(0,0,0,.45);"></div>`,
    })
    marker.on('click', () => {
      if (!infoWindow) infoWindow = new window.AMap.InfoWindow({ offset: new window.AMap.Pixel(0, -12) })
      infoWindow.setContent(infoHtml(p))
      infoWindow.open(map, [p.longitude, p.latitude])
      emit('marker-click', p)
    })
    map.add(marker)
    markers.push(marker)
  }
}

// 路线折线：先清旧线再画新线，并自动缩放适配整条路线
function renderRoute() {
  if (!map) return
  if (polyline) {
    map.remove(polyline)
    polyline = null
  }
  const path = props.routePath
    .filter((p) => p.longitude && p.latitude)
    .map((p) => [p.longitude, p.latitude])
  if (path.length < 2) return

  polyline = new window.AMap.Polyline({
    path,
    strokeColor: props.lineColor,
    strokeWeight: 6,
    strokeOpacity: 0.85,
    lineJoin: 'round',
    showDir: true,
  })
  map.add(polyline)
  map.setFitView([polyline], false, [60, 60, 60, 60], 16)
}

// 路线列表点击某个景点后，地图定位到该标记点
function focusMarker(id: string) {
  if (!map || !id) return
  const p = props.points.find((x) => x.id === id)
  if (!p || !p.longitude) return
  map.setZoomAndCenter(16, [p.longitude, p.latitude])
}

function initMap() {
  if (!window.AMap || !mapRef.value) {
    loadFailed.value = true
    return
  }
  map = new window.AMap.Map(mapRef.value, {
    zoom: MAP_ZOOM,
    center: MAP_CENTER,
    viewMode: '2D',
    resizeEnable: true,
  })
  map.addControl(new window.AMap.Scale())
  map.addControl(new window.AMap.ToolBar({ position: 'RB' }))
  renderMarkers()
  renderRoute()
}

onMounted(() => {
  // 高德脚本在 index.html 中同步加载，网络慢时轮询等待
  let tries = 0
  const timer = window.setInterval(() => {
    if (window.AMap) {
      window.clearInterval(timer)
      initMap()
    } else if (++tries > 50) {
      window.clearInterval(timer)
      loadFailed.value = true
    }
  }, 100)
})

onUnmounted(() => {
  if (map) map.destroy()
  map = null
})

watch(() => props.points, renderMarkers, { deep: true })
watch(() => props.routePath, renderRoute, { deep: true })
watch(() => props.lineColor, renderRoute)
watch(() => props.focusId, focusMarker)
</script>

<style scoped>
.amap-wrap {
  position: relative;
  width: 100%;
}

.amap {
  width: 100%;
  height: 100%;
}

.map-fallback {
  position: absolute;
  inset: 0;
  display: flex;
  align-items: center;
  justify-content: center;
  color: #909399;
  font-size: 13px;
  background: #f5f7fa;
}
</style>
