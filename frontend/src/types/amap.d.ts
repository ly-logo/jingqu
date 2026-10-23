export {}

declare global {
  interface Window {
    // 高德地图 JS API 2.0 全局对象（由 index.html 的 <script> 注入）
    AMap: any
    _AMapSecurityConfig?: { securityJsCode: string }
  }
}
