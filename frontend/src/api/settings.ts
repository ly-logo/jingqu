import http from './http'
import type { ApiResult, SystemSettings } from './types'

export const settingsApi = {
  get(): Promise<ApiResult<SystemSettings>> {
    return http.get('/settings').then((r) => r.data)
  },
  update(data: Partial<SystemSettings>): Promise<ApiResult<SystemSettings>> {
    return http.put('/settings', data).then((r) => r.data)
  },
}
