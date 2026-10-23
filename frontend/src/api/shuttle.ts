import http from './http'
import type { ApiResult, Shuttle } from './types'

export const shuttleApi = {
  list(): Promise<ApiResult<Shuttle[]>> {
    return http.get('/shuttles').then((r) => r.data)
  },
}
