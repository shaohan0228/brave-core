// Copyright (c) 2019 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

import * as React from 'react'
import { bindActionCreators, Dispatch } from 'redux'
import { connect } from 'react-redux'

// Components
import NewPrivateTabPage from './privateTab'
import NewTabPage from './newTab'

// Utils
import * as newTabActions from '../actions/new_tab_actions'
import * as gridSitesActions from '../actions/grid_sites_actions'
import * as binanceActions from '../actions/binance_actions'
import * as rewardsActions from '../actions/rewards_actions'
import * as geminiActions from '../actions/gemini_actions'
import * as bitcoinDotComActions from '../actions/bitcoin_dot_com_actions'
import * as cryptoDotComActions from '../actions/cryptoDotCom_actions'
import * as todayActions from '../actions/today_actions'
import * as PreferencesAPI from '../api/preferences'

// Types
import { NewTabActions } from '../constants/new_tab_types'

interface Props {
  actions: NewTabActions
  newTabData: NewTab.State
  gridSitesData: NewTab.GridSitesState
  braveTodayData: NewTab.BraveTodayState
}

class DefaultPage extends React.Component<Props, {}> {
  render () {
    const { newTabData, braveTodayData, gridSitesData, actions } = this.props

    // don't render if user prefers an empty page
    if (this.props.newTabData.showEmptyPage && !this.props.newTabData.isIncognito) {
      return <div />
    }

    return this.props.newTabData.isIncognito
      ? <NewPrivateTabPage newTabData={newTabData} actions={actions} />
      : (
        <NewTabPage
          newTabData={newTabData}
          todayData={braveTodayData}
          gridSitesData={gridSitesData}
          actions={actions}
          saveShowBackgroundImage={PreferencesAPI.saveShowBackgroundImage}
          saveShowStats={PreferencesAPI.saveShowStats}
          saveShowRewards={PreferencesAPI.saveShowRewards}
          saveShowTogether={PreferencesAPI.saveShowTogether}
          saveShowBinance={PreferencesAPI.saveShowBinance}
          saveShowAddCard={PreferencesAPI.saveShowAddCard}
          saveShowGemini={PreferencesAPI.saveShowGemini}
          saveShowBitcoinDotCom={PreferencesAPI.saveShowBitcoinDotCom}
          saveShowCryptoDotCom={PreferencesAPI.saveShowCryptoDotCom}
          saveBrandedWallpaperOptIn={PreferencesAPI.saveBrandedWallpaperOptIn}
        />
      )
  }
}

const mapStateToProps = (state: NewTab.ApplicationState): Partial<Props> => ({
  newTabData: state.newTabData,
  gridSitesData: state.gridSitesData,
  braveTodayData: state.today
})

const mapDispatchToProps = (dispatch: Dispatch): Partial<Props> => {
  const allActions = Object.assign({}, newTabActions, gridSitesActions, binanceActions, rewardsActions, geminiActions, bitcoinDotComActions, cryptoDotComActions, todayActions)
  return {
    actions: bindActionCreators(allActions, dispatch)
  }
}

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(DefaultPage)
