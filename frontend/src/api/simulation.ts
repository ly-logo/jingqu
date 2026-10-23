import http from './http'
import type { ApiResult, SimulationConfig, SimulationState } from './types'

export const simulationApi = {
  state(): Promise<ApiResult<SimulationState>> {
    return http.get('/simulation').then((r) => r.data)
  },
  start(): Promise<ApiResult<SimulationState>> {
    return http.post('/simulation/start').then((r) => r.data)
  },
  stop(): Promise<ApiResult<SimulationState>> {
    return http.post('/simulation/stop').then((r) => r.data)
  },
  updateConfig(config: Partial<SimulationConfig>): Promise<ApiResult<SimulationConfig>> {
    return http.put('/simulation/config', config).then((r) => r.data)
  },
}
