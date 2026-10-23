import http from './http'
import type { ApiResult, SystemMonitor } from './types'

export const monitorApi = {
  status(): Promise<ApiResult<SystemMonitor>> {
    return http.get('/system/monitor').then((r) => r.data)
  },
}
