import http from './http'
import type { ApiResult, StatisticsData } from './types'

export const statisticsApi = {
  overview(range?: string): Promise<ApiResult<StatisticsData>> {
    return http.get('/statistics', { params: { range } }).then((r) => r.data)
  },
}
