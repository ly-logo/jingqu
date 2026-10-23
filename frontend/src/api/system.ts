import http from './http'
import type { ApiResult, SystemStatus } from './types'

export const systemApi = {
  status(): Promise<ApiResult<SystemStatus>> {
    return http.get('/system/status').then((r) => r.data)
  },
}
